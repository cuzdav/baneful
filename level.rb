require 'ruby2d'
require_relative 'grid.rb'
require_relative 'gamestate.rb'
require_relative 'ruleui.rb'
require_relative 'solve2.rb'

class Level
  attr_reader :ruleui
  attr_reader :grid
  attr_reader :game_state
  attr_reader :rows
  attr_reader :eff_col
  attr_reader :cur_row
  attr_reader :color_map
  attr_reader :solver

  def initialize(level, num_moves, maxrows, maxwidth, x1, y1, x2, y2)
    @cur_row = 0
    @rules = level[:rules]
    @rows = level[:rows]
    @num_moves = num_moves
    @max_width = maxwidth
    @eff_col = 0

    @color_map = make_color_map(level)
    make_playarea_rows(level, maxrows, maxwidth, x1, y1, x2, y2)
    make_rules()
    next_gamestate()
  end

  def next_gamestate()
    row_str = @cur_row >= 0 ? @rows[@cur_row] : ""
    @game_state = GameState.new(@rules, row_str, @num_moves, @max_width)
  end

  # grid index of pixel coord
  def active_x1_coord(col)
    return @grid.xcoord(col)
  end

  # grid index of pixel coord
  def active_x2_coord(col)
    return @grid.xcoord(col) + @grid.cell_width
  end

  # grid index of pixel coord
  def active_y1_coord()
    return @grid.ycoord(@cur_row)
  end

  # grid index of pixel coord
  def active_y2_coord()
    return @grid.ycoord(@cur_row) + @grid.cell_height
  end

  def update_after_modification
    update_grid_row(@cur_row, @game_state.cur_row)
    if @game_state.solved?
      @cur_row -= 1
      if @cur_row < 0
        next_level()
      end
      next_gamestate
      update_grid_row(@cur_row, @game_state.cur_row)
    end
  end

  def reset_cur_level
    @game_state.reset
    update_grid_row(@cur_row, @game_state.cur_row)
  end

  def undo_move
    @game_state.undo_move
    update_grid_row(@cur_row, @game_state.cur_row)
  end

  private

  def make_color_map(level_cfg)
    color_map = {}
    color_idx = -1
    # build mapping from char -> color
    level_cfg[:rules].each do |from, tolist|
      from.each_char do|c|
        if not SPECIAL_PATTERN_CHARS.include?(c)
          color_map[c] ||= COLORS[color_idx += 1]
        end
      end
      tolist.each do|to|
        to.each_char do |c|
          if not SPECIAL_REPL_CHARS.include?(c)
            color_map[c] ||= COLORS[color_idx += 1]
          end
        end
      end
    end
    level_cfg[:rows].each do |row|
      row.each_char do |c|
        color_map[c] ||= COLORS[color_idx += 1]
      end
    end

    return color_map
  end

  def make_playarea_rows(level, maxrows, maxwidth, x1, y1, x2, y2)
    verify_rows
    @numrows = level[:rows].size
    height = (y2 - y1).abs
    max_cell_height = height / maxrows

    eff_y2 = (height * 0.6).to_i
    eff_y1 = eff_y2 - @numrows * max_cell_height
    if eff_y1 < 0
      eff_y2 += eff_y1.abs
      eff_y1 = 0
    end

    @numcols = maxwidth
    @grid = Grid.new(@numrows, maxwidth, x1, eff_y1, x2, [y2, eff_y2].min)
    @grid.remove_all()

    @grid.highlight_background
    @grid.set_background_color(Color.new([10, 10, 10, 0.15]))

    # add each row to this level
    opacity = 0.6
    level[:rows].each do |row|
      update_grid_row(@cur_row, row, opacity)
      @cur_row += 1
    end
    @cur_row -= 1

    (0...@numcols).each do |col|
      @grid.set_cell_opacity(@cur_row, col, 1) # reset playing level opacity to 1
    end

  end

  # initialize or update after a modification the given logical rownum
  # with row string.
  def update_grid_row(rownum, row_str, opacity=1)
    effrow = rownum

    # center on grid
    effcol = @numcols/2 - row_str.size / 2
    @eff_col = effcol

    hide_cells_in_row(rownum, 0, @eff_col) # leading empty cells
    row_str.each_char do |ch|
      puts("update_grid_row: ch=#{ch} (#{ch.ord})")
      @grid.set_cell_color(effrow, effcol, @color_map[ch]).add
      @grid.set_cell_opacity(effrow, effcol, opacity)
      effcol += 1
    end
    hide_cells_in_row(rownum, effcol, @numcols) # trailing empty cells
  end

  def hide_cells_in_row(rownum, from, to)
    (from...to).each do |x|
      @grid.hide_cell(rownum, x)
    end
  end

  def make_rules()
    @ruleui = RuleUI.new(@color_map, @rules)

    window_width = Window.get(:width)
    window_height = Window.get(:height)
    width = [
      window_width - 2 * HORIZ_RULE_OFFSET_PX,
      @grid.cell_width * @ruleui.num_cols
    ].min
    height = [
      RULE_AREA_HEIGHT_PX,
      @grid.cell_height * @ruleui.num_rows
    ].min

    x1 = window_width / 2 - width / 2 + HORIZ_RULE_OFFSET_PX
    x2 = window_width / 2 + width / 2 - HORIZ_RULE_OFFSET_PX
    y1 = playarea_height + VERT_RULE_OFFSET_PX
    y2 = window_height - VERT_RULE_OFFSET_PX
    @ruleui.resizing_move_to(x1, y1, x2, y2)
  end

  def verify_rows()
    @solver = Solver.new(@rules, @num_moves, @max_width)
    @rows.each do |row_str|
      game_state = GameState.new(@rules, row_str, @num_moves, @max_width)
      if @solver.find_solution(game_state) == nil
        raise("Row #{row_str} is not solvable")
      end
    end
  end
end

