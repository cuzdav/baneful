require 'ruby2d'
require_relative 'grid.rb'
require_relative 'gamestate.rb'
require_relative 'ruleui.rb'
require_relative 'solve2.rb'

class CellFactory
  def initialize(level_cfg, color_map, move_number_provider)
    @color_map = color_map
    @type_map = {}
    @init_args_map = {}
    @color_map.keys.each do |ch|
      @type_map[ch] = Rectangle
      @init_args_map[ch] = []
    end

    type_overrides = level_cfg[:type_overrides]
    if type_overrides != nil
      init_type_overrides(type_overrides, move_number_provider)
    end
  end

  def get_type_for_char(ch)
    return @type_map[ch]
  end

  def create(ch)
    args = @init_args_map[ch]
    return get_type_for_char(ch).new(*args)
  end

  private

  def init_type_overrides(type_overrides, move_num_provider)
    type_overrides.each do |override_ch, override_ch_config|
      typename = override_ch_config["type"]
      type =
        case (typename)
        when "RotatingColors"
          cycle_chars = override_ch_config["cycle_chars"]
          if cycle_chars == nil
            raise "Missing 'cycle_chars' from type_override for " +
                  "#{override_ch} in level: #{level_cfg[:name]}\n\t==>#{override_ch_config}"
          end
          args = [move_num_provider]
          cycle_chars.each_char do |ch|
            args << @color_map[ch]
          end
          @init_args_map[override_ch] = args
          RotatingColorsWigit
        when "Rectangle"
          Rectangle
        else
          raise "Unhandled type #{typename}"
        end
      puts ("Setting type of '#{override_ch}' to #{type}")
      @type_map[override_ch] = type
    end
  end
end


class Level
  attr_reader :ruleui
  attr_reader :grid
  attr_reader :game_state
  attr_reader :rows
  attr_reader :eff_col
  attr_reader :cur_row
  attr_reader :color_map
  attr_reader :solver
  attr_reader :name

  def initialize(level, num_moves, maxrows, maxwidth, x1, y1, x2, y2)
    @cur_row = 0
    @rules = level[:rules]
    @rows = level[:rows]
    @type_overrides = level[:type_overrides]
    @num_moves = num_moves
    @max_width = maxwidth
    @eff_col = 0
    @name = level[:name] || ""

    @color_map = make_color_map(level)
    @cell_factory = CellFactory.new(level, @color_map, self)
    make_playarea_rows(level, maxrows, maxwidth, x1, y1, x2, y2)
    make_rules()
    next_gamestate()
  end

  def next_gamestate()
    row_str = @cur_row >= 0 ? @rows[@cur_row] : ""
    @game_state = GameState.new(
      @rules, row_str, @type_overrides, @max_width)
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

  def cur_move_number()
    return @game_state ? @game_state.cur_move_number : 0
  end

  def update_after_modification
    @needs_modify_callback = update_grid_row(@cur_row, @game_state.cur_raw_row)
    if @game_state.solved?
      @cur_row -= 1
      if @cur_row < 0
        next_level()
      end
      next_gamestate
      @needs_modify_callback = update_grid_row(@cur_row, @game_state.cur_raw_row)
    end
    @needs_modify_callback.each do |cell|
      cell.modified
    end
  end

  def reset_cur_level
    @game_state.reset
    update_after_modification
  end

  def undo_move
    @game_state.undo_move
    update_after_modification
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

    eff_y2 = height
    eff_y1 = eff_y2 - @numrows * max_cell_height
    if eff_y1 < 0
      eff_y2 += eff_y1.abs
      eff_y1 = 0
    end

    @numcols = maxwidth
    @grid = Grid.new(@numrows, maxwidth, x1, eff_y1, x2, eff_y2)
    @grid.remove_all()

    @grid.highlight_background
    @grid.set_background_color(Color.new([10, 10, 10, 0.15]))

    # add each row to this level
    opacity = 0.6
    level[:rows].each do |row|
      needs_update = update_grid_row(@cur_row, row, opacity)
      @needs_update_callback = needs_update
      @cur_row += 1
    end
    @cur_row -= 1

    (0...@numcols).each do |col|
      @grid.set_cell_opacity(@cur_row, col, 1) # reset playing level opacity to 1
    end

    @grid.refresh()

  end

  # initialize or update after a modification the given logical rownum
  # with row string.
  def update_grid_row(effrow, row_str, opacity=1)
    # center on grid
    effcol = @numcols/2 - row_str.size / 2
    @eff_col = effcol

    hide_cells_in_row(effrow, 0, effcol) # leading empty cells

    needs_modify_callback = []
    row_str.each_char do |ch|
      cell_object = init_grid_cell(ch, effrow, effcol, opacity)
      if cell_object.needs_modified_callback
        needs_modify_callback << cell_object
      end
      effcol += 1
    end
    hide_cells_in_row(effrow, effcol, @numcols) # trailing empty cells

    return needs_modify_callback
  end

  #
  # For each playarea cell.  Complication now that there are custom
  # wigits in the playarea.  Abstracted to a factory.
  #
  def init_grid_cell(ch, effrow, effcol, opacity)
    # see if object is already the type we expect
    type = @cell_factory.get_type_for_char(ch)
    cell_object = @grid.get_cell_object(effrow, effcol)
    if cell_object.class != type
      # nope, must create
      cell_object = @cell_factory.create(ch)
    end
    @grid.set_cell_object(effrow, effcol, cell_object)
    @grid.set_cell_color(effrow, effcol, @color_map[ch])
    @grid.set_cell_opacity(effrow, effcol, opacity)
    cell_object.z = 10

    return cell_object
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

    # adjustable height, try to keep same top unless necessary to move up
    cell_height = @grid.cell_height + @grid.vert_gap_px
    rulearea_height = [
      rulearea_max_height(),
      cell_height * @ruleui.num_rows
    ].min

    x1 = window_width / 2 - width / 2 + HORIZ_RULE_OFFSET_PX
    x2 = window_width / 2 + width / 2 - HORIZ_RULE_OFFSET_PX

    y1 = window_height - rulearea_height - VERT_RULE_OFFSET_PX
    y2 = y1 + rulearea_height
    @ruleui.resizing_move_to(x1, y1, x2, y2)
  end

  def verify_rows()
    @solver = Solver.new(@rules, @num_moves, @max_width)
    @rows.each do |row_str|
      game_state = GameState.new(@rules, row_str, @type_overrides, @max_width)
      if @solver.find_solution(game_state).empty?
        raise("Row #{row_str} is not solvable")
      end
    end
  end
end

