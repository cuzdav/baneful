require 'ruby2d'
require_relative 'gameover.rb'
require_relative 'hint.rb'

TGT_START_X = 0
TGT_END_X = 1
TGT_COL = 2

class InputState

  attr_accessor :selected_target_idx

  def initialize(bg_music)
    @bg_music = bg_music
    @ruleui = nil
    @cur_level = nil
    @selected_rule = nil
    @selected_row = nil
    @selected_target_idx = nil
    @mousedown = false
    @lines_free = []
    @quads_free = []
    @possible_plays = nil
    @hint = Hint.new(self)

    # rule pattern to data associated:
    #  :wigits add to Window to indicate rule is active
    #  :target_cell_xcoords: when locked&loaded, these x-ranges will
    #      select the potential replacement cells in plary area
    @rule_data = Hash.new()
  end

  def no_more_moves
    if @cur_level.game_state.solved?
      Text.new("You Win", size: 80, color: "white", z:20)
    else
      @game_over_state = GameOverState.new
    end
  end

  def prepare_next_level(ruleui, cur_level)
    @ruleui = ruleui
    @cur_level = cur_level
    update_from_game_state
  end

  #
  # precompute all possible moves, build "spotlights" to the
  # targets. Should be called after the gamestate changes
  def update_from_game_state
    @possible_plays = @cur_level.game_state.possible_plays || []

    puts("Possible plays: #{@possible_plays}")

    @rule_data.each do |pat, data|
      data[:wigits].each do |v|
        @quads_free << v
      end
    end
    @rule_data.clear

    rule_gap = @ruleui.gap_px
    grid_first_col_offset = @cur_level.eff_col

    @ruleui.rules.each do |rule|
      rule_x1 = rule.x1 - rule_gap / 2
      rule_y1 = rule.y1
      rule_x2 = rule.x2 + rule_gap / 2
      rule_y2 = rule_y1
      has_moves = false
      repl_has_moves = []
      repl_idx = 0
      rule.replacement_strs.each do |rep_str|
        quads = []
        target_cell_xcoords = []

        # all possible plays from the current rule
        plays = @possible_plays.select do |p|
          p[GS_PLAY_REPL] == rep_str and p[GS_PLAY_PAT] == rule.from_str
        end

        repl_has_moves[repl_idx + FIRST_REPL_ROW] = !plays.empty?

        plays.each do |idx, pat, repl|
          #idx is 0-based, while we want it to be grid-
          idx += grid_first_col_offset

          has_moves = true
          pat_x1 = @cur_level.active_x1_coord(idx)
          pat_y1 = @cur_level.active_y2_coord() # bottom edge of row's y
          pat_x2 = @cur_level.active_x1_coord(idx + pat.size)
          pat_y2 = pat_y1

          move_data = [pat_x1, pat_x2, idx]
          target_cell_xcoords << move_data

          quad = get_quad
          quad.x1 = rule_x1
          quad.x2 = rule_x2
          quad.x3 = pat_x2
          quad.x4 = pat_x1
          quad.y1 = rule_y1
          quad.y2 = rule_y2
          quad.y3 = pat_y1
          quad.y4 = pat_y2
          quad.color = "white"
          quad.opacity = 0.2
          quads << quad
        end

        @rule_data[rule.from_str] = {
          :wigits              => quads,
          :target_cell_xcoords => target_cell_xcoords,
          :has_moves           => has_moves,
          :row_has_moves      => repl_has_moves
        }
        repl_idx += 1
      end

      opacity = has_moves ? 1 : 0.6
      rule.rule_grid.foreach_rect_with_index do |rect, row, col|
        if row == 0
          rect.opacity = opacity
        elsif row >= FIRST_REPL_ROW
          rect.opacity = repl_has_moves[row] ? 1 : 0.6
        end
      end
    end

    if @possible_plays.empty?
      puts(">>>>>>>> No more moves <<<<<<<<")
      no_more_moves
    end
  end

  def apply_move(grid_idx, from_str, to_str)
    idx = grid_idx - @cur_level.eff_col

    # CHANGE OFFICIAL GAME STATE (attempt)
    result = @cur_level.game_state.make_move(idx, from_str, to_str)

    if result == nil
      puts("Curr game state: #{@cur_level.game_state.cur_row}xo")
      puts("idx:#{idx}, from:#{from_str}, to:#{to_str}")
      puts("PROBLEM??? applying move failed!")
    end

    @cur_level.update_after_modification
    update_from_game_state()
  end

  def on_mouse_down(event)
    # If they double-click on a row, it unselects the row, then
    # we have rule selected but not a selected_row, and it causes
    # problems.  So this resets the selected state if the mouse
    # is over a just-unselected box that should still be selected.
    on_mouse_move(event)

    case event.button
    when :left
      on_mouse_left_down(event)
    when :right
      on_mouse_right_down(event)
    end
  end

  def on_mouse_left_down(event)
    @mousedown = true
    if @selected_rule != nil
      replacement_str = @ruleui.get_replacement_str_at(event.x, event.y)
      if @selected_row != nil and replacement_str != nil
        @selected_rule.rule_grid.select_row(@selected_row, 'yellow', 5)
      end
    end
  end

  def on_key_down(event)
    case event.key
    when "left shift", "right shift"
      @shiftdown = true
    when "left ctrl", "right ctrl"
      @ctrldown = true
    end
  end

  def on_key_up(event)
#    puts "KEYDOWN:"
#    p event
    case event.key
    when "escape"
      unselect_replacement
    when "h"
      @hint.next_hint(@cur_level)
      if @hint.solution != nil
        gs = @cur_level.game_state.clone_from_cur_position
        puts("Start")
        puts gs.cur_row
        @hint.solution.each do |idx, from, to|
          gs.make_move(idx, from, to)
          puts("#{idx}: #{from}->#{to}   ==> #{gs.cur_row} ")
        end
      end
      unselect_playarea_grid
    when "r"
      restart
    when "u"
      undo_move()
    when "="
      if @shiftdown
        @bg_music.volume_up(5)
      end
    when "-"
        @bg_music.volume_down(5)

    when 'p'
      if @paused
        @bg_music.resume
        @paused = false
      else
        @bg_music.pause
        @paused = true
      end

    when '.'
      @cur_level.game_state.cur_row = ""
      if @shiftdown
        unselect_rule
        @cur_level.update_after_modification
        update_from_game_state
      end

    when "]"
      puts("calling next track")
      @bg_music.next_track

    when "left shift", "right shift"
      @shiftdown = false
    when "left ctrl", "right ctrl"
      @ctrldown = false
    end
  end

  def on_mouse_up(event)
    case event.button
    when :left
      on_mouse_left_up(event)
    when :right
      on_mouse_right_up(event)
    end
    on_mouse_move(event) # reset positioning
  end

  def on_mouse_left_up(event)
    @mousedown = false
    if @game_over_state != nil
      restart()
    elsif @selected_target_idx != nil
      apply_choice_maybe
      @locked_row = nil
      @locked_rule = nil
      unselect_playarea_grid
      unselect_rule
    elsif @selected_row != nil
      @locked_row = @selected_row
      @locked_rule = @selected_rule
    end
  end

  def apply_choice_maybe
    if @selected_rule != nil
      from = @selected_rule.from_str
      to = @selected_repl_str
      idx = @selected_target_idx
      unselect_replacement()
      unselect_playarea_grid

      if idx != nil
        apply_move(idx, from, to)
      end
    end
    unselect_rule
  end

  def on_mouse_right_up(event)
  end

  def on_mouse_right_down(event)
    unselect_replacement
    unselect_playarea_grid
    unselect_rule
    @locked_row = nil
    @locked_rule = nil
  end

  def undo_move
    @cur_level.undo_move
    if @game_over_state != nil
      @game_over_state.clear
      @game_over_state = nil
    end
    update_from_game_state()
  end

  def restart
    @cur_level.reset_cur_level
    if @game_over_state != nil
      @game_over_state.clear
      @game_over_state = nil
    end
    update_from_game_state()
  end

  def on_mouse_move(event)
    if @locked_row != nil
      mouse_targeting(event.x, event.y)
    elsif event.y >= @ruleui.y1
      mouse_over_rule(event)
    elsif not @mousedown
      unselect_rule
    end
  end

  def mouse_targeting(mousex, mousey)
    start_col = nil

    # go over all possible targets in the play area, find which, if any,
    # mouse is targeting.

    # note: cells in grid are drawn centered, so grid coords may not
    # have same index as "string" coords.  Example:
    # String: "XXY" and grid width is 5, grid may be [_XXY_]
    # XXY string coords (in gamestate) are 0, but in grid are 1.
    # @target_cell_coords are grid-relative, not string relative.
    # This offset is @cur_level.eff_col

    if @locked_row != nil and not @target_cell_coords.empty? and @selected_target_idx == nil
      start_col = @target_cell_coords[0][TGT_COL]
    end
    @target_cell_coords.each do |x1, x2, col|
      start_col = col if mousex > x1 and mousex < x2
    end
    if start_col != nil
      start_col
      if @selected_target_idx != start_col
        if @selected_target_idx != nil
          @cur_level.grid.unselect
        end
        @selected_target_idx = start_col
        @cur_level.grid.select_cells(
          @cur_level.cur_row,
          start_col,
          start_col + @locked_rule.from_str.size - 1)
      end
    end
  end

  def mouse_over_rule(event)
    if not @mousedown
      x = event.x
      y = event.y
      rule = @ruleui.get_rule_at(x, y)

      select_rule(rule)

      if rule != nil
        row, _ = rule.rowcol_for_coord(x, y)
        if @selected_row != row || rule.replacement_strs.size == 1
          unselect_replacement
          if row != nil
            select_replacement(row, x, y)
          end
        end
      end
    end
  end

  def select_rule(rule)
    if rule != nil
      if rule != @selected_rule
        unselect_rule
      end
      @selected_rule = rule
      data = @rule_data[rule.from_str]
      rule.rule_grid.highlight_background if data[:has_moves]
    end
  end

  def select_replacement(row, mousex, mousey)
    data = @rule_data[@selected_rule.from_str]
    row_has_moves = data[:row_has_moves]
    maxrow = @selected_rule.replacement_strs.size + FIRST_REPL_ROW

    if row != nil and row_has_moves[row]
      # weird, when compiled this is float, interpreted, it's an int
      row = row.floor

      repl_str = @selected_rule.get_replacement_str_for_row(row)
      @selected_row = row
      @selected_repl_str = repl_str
      @selected_rule.rule_grid.select_row(row, 'yellow', 4)

      @target_cell_coords = data[:target_cell_xcoords]
      wigits = data[:wigits]
      wigits.each do |quad|
        quad.add
      end
    end
  end

  def unselect_replacement
    unselect_wigits
    return if @selected_rule == nil
    @selected_rule.rule_grid.unselect if @selected_rule != nil
    @selected_repl_str = nil
    @selected_row = nil
    unselect_playarea_grid
  end

  def unselect_playarea_grid
    @selected_target_idx = nil
    @cur_level.grid.unselect
  end

  def unselect_rule
    if @selected_rule != nil
      @hint.clear
      @selected_rule.rule_grid.unhighlight_background
      unselect_replacement
      unselect_wigits
      @selected_rule_has_moves = false
      @selected_rule.rule_grid.unselect
      @selected_rule = nil
    end
  end

  def unselect_wigits
    if @rule_data != nil and @selected_rule != nil
      data = @rule_data[@selected_rule.from_str]
      if data != nil
        wigits = data[:wigits]
        if wigits != nil
          wigits.each do |quad|
            quad.remove
          end
        end
      end
    end
  end

  def get_quad
    quad = @quads_free.pop || Quad.new
    quad.remove
    return quad
  end

  def release_quad(quad)
    @quads_free.pop << quad
  end
end
