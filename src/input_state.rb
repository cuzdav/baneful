require 'ruby2d'
require_relative 'gameover.rb'
require_relative 'hint.rb'

TGT_START_X = 0
TGT_END_X = 1
TGT_COL = 2
TGT_MOVE = 3


class InputState

  attr_accessor :selected_target_idx

  def initialize(bg_music)
    @bg_music = bg_music
    @ruleui = nil
    @cur_level = nil
    @selected_rule = nil
    @selected_repl = nil
    @selected_target_idx = nil
    @mousedown = false
    @lines_free = []
    @quads_free = []
    @possible_plays = nil
    @level_name_wigit = Text.new("", x:5, y:5, z:20, color:"white", size:18)

    # [rule_data]
    # rule pattern to data associated:
    #  :wigits add to Window to indicate rule is active
    #  :target_cell_info: when locked&loaded, these x-ranges will
    #      select the potential replacement cells in play area
    @rule_data = Hash.new()
  end

  def no_more_moves
    if @cur_level.game_state.solved?
      Text.new("You Win", size: 80, color: "white", y: 30, z:20)
    else
      @game_over_state = GameOverState.new
    end
  end

  def prepare_next_level(ruleui, cur_level, solver)
    @hint.clear if @hint != nil
    @hint = Hint.new(self, solver)
    @ruleui = ruleui
    @cur_level = cur_level
    @level_name_wigit.text = cur_level.name
    update_from_game_state
  end

  #
  # precompute all possible moves for current position, build "spotlights" to
  # the targets. Should be called after the gamestate changes
  def update_from_game_state
    @possible_plays = @cur_level.game_state.possible_plays || []

    puts("Possible plays: #{@possible_plays}")

    @rule_data.each do |pat, data|
      data[:wigits].each do |wigits_per_row|
        if wigits_per_row != nil
          wigits_per_row.each do |wigit|
            wigit.remove
          end
        end
      end
    end
    @rule_data.clear

    rule_gap = @ruleui.gap_px

    #
    # Build up rep for current state of each rule
    #
    @ruleui.rules.each do |rule|
      rule_x1 = rule.x1 - rule_gap / 2
      rule_y1 = rule.y1
      rule_x2 = rule.x2 + rule_gap / 2
      rule_y2 = rule_y1

      wigits_per_row = []
      target_cell_info_per_row = []
      row_has_moves = []
      has_moves = false

      rule_data = {
        wigits: wigits_per_row,
        has_moves: false,
        target_cell_info: target_cell_info_per_row,
        row_has_moves: row_has_moves,
      }
      @rule_data[rule.from_str] = rule_data

      row_idx = FIRST_REPL_ROW
      rule.replacement_strs.each do |rep_str|
        quads = []
        row_info = []
        wigits_per_row[row_idx] = quads
        target_cell_info_per_row[row_idx] = row_info

        # filter all possible plays from the current rule to just those of
        # this pattern and replacement string
        plays = @possible_plays.select do |p|
          repl = p[GS_PLAY_REPL]
          captures = p[GS_PLAY_CAPTURES]
          wc_equals(p[GS_PLAY_PAT], rule.from_str) and
            wc_with_placeholder_equals(repl, rep_str, captures)
        end
        row_has_moves[row_idx] = !plays.empty?
        has_moves = rule_data[:has_moves] |= !plays.empty?

        #
        # Store the move data for each play, and the quad (for the spotlight)
        # in per-row data, which is in the per-rule data
        #
        plays.each do |move|
          #idx is 0-based, while we want it to be grid-based
          idx = move[GS_PLAY_IDX]
          pat = move[GS_PLAY_PAT]
          grid_idx = idx + @cur_level.eff_col
          pat_x1 = @cur_level.active_x1_coord(grid_idx)
          pat_y1 = @cur_level.active_y2_coord() # bottom edge of row's y
          pat_x2 = @cur_level.active_x1_coord(grid_idx + pat.size)
          pat_y2 = pat_y1
          move_data = [pat_x1, pat_x2, grid_idx, move]
          row_info << move_data

          quad = Quad.new
          quad.remove
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

        row_idx += 1
      end

      # show rule pattern, rows, as lit or dim, based on if they are playable.
      # (Pattern is playable if any replacement is playable.  Some replacements
      # are not playable because they might make the row too wide.)
      opacity = has_moves ? 1 : 0.6
      rule.rule_grid.foreach_rect_with_index do |rect, row, col|
        if row == 0
          # pattern
          rect.opacity = opacity
        elsif row >= FIRST_REPL_ROW
          # replacement row(s)
          rect.opacity = row_has_moves[row] ? 1 : 0.6
        end
      end
    end

    if @possible_plays.empty?
      puts(">>>>>>>> No more moves <<<<<<<<")
      no_more_moves
    end
  end

  def apply_move(move)
    # CHANGE OFFICIAL GAME STATE (attempt)
    result = @cur_level.game_state.make_move(move)

    if result == nil
      puts("Curr game state: #{@cur_level.game_state.cur_row}xo")
      puts("MOVE: #{move}")
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
      if @selected_repl != nil and replacement_str != nil
        @selected_rule.rule_grid.select_row(@selected_repl, 'yellow', 5)
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
      give_hint

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
      toggle_music_paused

    when 'q'
      if @ctrldown
        exit
      end

    when '.'
      @cur_level.game_state.cur_row = ""
      if @shiftdown
        unselect_rule
        @cur_level.update_after_modification
        update_from_game_state
      end

    when "]"
      @bg_music.next_track

    when "left shift", "right shift"
      @shiftdown = false

    when "left ctrl", "right ctrl"
      @ctrldown = false
    end
  end

  def toggle_music_paused
    if @music_paused
      @bg_music.resume
      @music_paused = false
    else
      @bg_music.pause
      @music_paused = true
    end
  end

  def give_hint
    @hint.next_hint(@cur_level)
    if @hint.solution != nil
      gs = @cur_level.game_state.clone_from_cur_position
      puts("Start")
      puts gs.cur_row
      @hint.solution.each do |move|
        gs.make_move(move)
        puts("#{move[GS_PLAY_IDX]}: #{move[GS_PLAY_PAT]}->#{move[GS_PLAY_REPL]}   ==> #{gs.cur_row} ")
      end
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
    elsif @selected_repl != @locked_repl
      @locked_repl = @selected_repl
      @locked_rule = @selected_rule
    elsif @selected_target_idx != nil
      apply_choice_maybe
      @locked_repl = nil
      @locked_rule = nil
      unselect_playarea_grid
      unselect_rule
    end
  end

  def apply_choice_maybe
    if @selected_rule != nil
      move = @selected_move
      unselect_replacement()
      unselect_playarea_grid
      if move != nil
        apply_move(move)
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
    @locked_repl = nil
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
    @selected_target_idx = nil
    @locked_repl = nil
    unselect_rule
    unselect_replacement
    unselect_playarea_grid


    if @game_over_state != nil
      @game_over_state.clear
      @game_over_state = nil
    end

    update_from_game_state()
  end

  def on_mouse_move(event)
    if @locked_repl != nil
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
    # @target_row_info are grid-relative, not string relative.
    # This offset is @cur_level.eff_col

    # start by selecting the potential leftmost target, then
    # scan for better mouse-coord matches
    if @locked_repl != nil and not @target_row_info.empty? and @selected_target_idx == nil
      first_target = @target_row_info[0]
      cur_move = first_target[TGT_MOVE]
      grid_col = first_target[TGT_COL]
    end

    # x1, x2 are mouse coord ranges, col is grid-relative index,
    # and move is hash from possible_plays, describing a move
    @target_row_info.each do |x1, x2, col, move|
      if mousex > x1 and mousex < x2
        grid_col = col
        cur_move = move
      end
    end
    select_target(grid_col, cur_move)
  end

  def select_target(grid_col, move)
    if grid_col != nil
      if @selected_target_idx != grid_col
        if @selected_target_idx != nil
          @cur_level.grid.unselect
        end
        @selected_target_idx = grid_col
        @selected_move = move
        @cur_level.grid.select_cells(
          @cur_level.cur_row,
          grid_col,
          grid_col + move[GS_PLAY_PAT].size - 1)
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
        if @selected_repl != row || rule.replacement_strs.size == 1
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
      # weird, when compiled with mruby, this is a float, but interpreted it's an int
      row = row.floor

      repl_str = @selected_rule.get_replacement_str_for_row(row)
      @selected_repl = row
      @selected_rule.rule_grid.select_row(row, 'yellow', 4)

      @target_row_info = data[:target_cell_info][row]
      wigits = data[:wigits][row]
      if wigits != nil
        wigits.each do |quad|
          quad.add
        end
      end
    end
  end

  def unselect_replacement
    unselect_wigits
    return if @selected_rule == nil
    @selected_rule.rule_grid.unselect if @selected_rule != nil
    @selected_repl = nil
    unselect_playarea_grid
  end

  def unselect_playarea_grid
    @selected_target_idx = nil
    @selected_move = nil
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
        data[:wigits].each do |wigit_row|
          if wigit_row != nil
            wigit_row.each do |quad|
              quad.remove
            end
          end
        end
      end
    end
  end
end