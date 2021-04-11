require 'ruby2d'
require_relative 'custom_widgets.rb'

FIRST_REPL_ROW = 2

# @rule_grid - obj containing rects for each cell in rule and replacements
class SingleRule
  attr_reader :num_cols
  attr_reader :num_rows # includes rule and blank row beneath it
  attr_reader :num_replacements
  attr_reader :from_str
  attr_reader :replacement_strs
  attr_reader :rule_grid

  # from_str is a string representing the "from" pattern
  # replacement_strs is an array of strings representing the "to" patterns
  def initialize(parent, color_map, from_str, replacement_strs, cell_factory)
    @parent = parent
    @from_str = from_str
    @replacement_strs = replacement_strs
    @num_replacements = replacement_strs.size
    @num_rows = @num_replacements + FIRST_REPL_ROW
    @num_cols = [from_str.size, *replacement_strs.map {|str| str.size}].max
    @rule_grid = Grid.new(@num_replacements + FIRST_REPL_ROW, @num_cols, 0,0,0,0)
    @rule_grid.clear
    @color_map = color_map

    # add colors for rules and replacements
    set_rule_source_cells(from_str, cell_factory)
    set_replacement_cells(from_str, replacement_strs, cell_factory)
  end


  def clear
    @rule_grid.clear
    @parent.hintbox.remove
  end

  def get_replacement_str_for_row(row)
    # row 0 = pattern, row 1 is blank, row 2 is first replacement, etc
    if row != nil and row >= FIRST_REPL_ROW
      return @replacement_strs[row - FIRST_REPL_ROW]
    end
    nil
  end

  def rowcol_for_coord(x, y)
    return @rule_grid.rowcol_for_coord(x, y)
  end

  def resizing_move_to(x1, y1, x2, y2)
    @rule_grid.resizing_move_to(x1, y1, x2, y2)
  end

  def x1
    @rule_grid.x1
  end

  def y1
    @rule_grid.y1
  end

  def x2
    @rule_grid.x2
  end

  def y2
    @rule_grid.y2
  end

  def index_of_repl(repl_str)
    idx = 0
    puts ("Searching for #{repl_str} in one of #{@replacement_strs}")
    @replacement_strs.each do |repl|
      return idx if repl == repl_str
      idx += 1
    end
    nil
  end

  def show_pattern_hint
    pat_hint = get_hintbox
    pat_hint.y = @rule_grid.y1
    pat_hint.z = 1
    pat_hint.add
  end

  # row: 0 based, in terms of config list, not grid offset
  def show_repl_hint(repl_str)
    idx = index_of_repl(repl_str)
    return if idx == nil

    repl_hint = get_hintbox
    repl_hint.y = @rule_grid.ycoord(idx + 2)
    repl_hint.z = 0
    repl_hint.add
  end

  def unselect_hints
    @parent.unselect_hints
  end

  private

  # given a string pattern, enable and colorize the cells in the grid
  def set_rule_source_cells(from_str, cell_factory)
    #TODO: use cell_factory
    row = 0
    col = 0
    num_wild = 0
    total_wild = from_str.count(".")
    from_str.each_char do |ch|
      if ch == '.'
        num_to_show = total_wild > 1 ? num_wild : nil
        num_wild += 1
        cell_obj = WildcardWidget.new(num_to_show)
      elsif (ch.ord >= ?0.ord and ch.ord <= ?9.ord)
        #TODO: make explicit placeholders ok too
        cell_obj = WildcardWidget.new(ch)
      else
        cell_obj = cell_factory.create(ch)
        color = @color_map[ch]
      end
      @rule_grid.set_cell_object(row, col, cell_obj)
      @rule_grid.set_cell_color(row, col, color)
      @rule_grid.set_cell_opacity(row, col, 0)
      @rule_grid.show_cell(row, col)
      col += 1
    end
  end

  def set_replacement_cells(from_str, replacement_strs, cell_factory)
    row = 2
    col = 0
    num_wildcards = from_str.count(".")
    replacement_strs.each do |repl_str|
      if not repl_str.empty?
        repl_str.each_char do |ch|
          if SPECIAL_REPL_CHARS.include?(ch)
            wc_num = num_wildcards > 1 ? ch.ord - ?1.ord : nil
            cell_obj = WildcardWidget.new(wc_num)
            @rule_grid.set_cell_object(row, col, cell_obj)
          else
            cell_obj = cell_factory.create(ch)
            @rule_grid.set_cell_object(row, col, cell_obj)
            if cell_obj.needs_modified_callback
              @parent.cells_needing_updates << cell_obj
            end
          end
          @rule_grid.show_cell(row, col)
          col += 1
        end
      else
        empty_cell = EmptyReplacementWidget.new('red')
        @rule_grid.set_cell_object(row, col, empty_cell)
        @rule_grid.show_cell(row, col)
      end
      row += 1
      col = 0
    end
  end

  private

  def get_hintbox
    hintbox = @parent.hintbox
    hintbox.x = (@rule_grid.x1 - @parent.gap_px / 2).floor
    hintbox.width = @rule_grid.x2 - @rule_grid.x1 + @parent.gap_px
    hintbox.height = @parent.cell_height + @parent.gap_px / 2
    hintbox.z = 0
    return hintbox
  end
end


#
# Collection and manager of SingleRule objects
#
class RuleUI
  attr_accessor :gap_px
  attr_accessor :single_rules
  attr_reader :hintbox
  attr_reader :x1
  attr_reader :y1
  attr_reader :x2
  attr_reader :y2
  attr_reader :num_rows
  attr_reader :num_cols
  attr_reader :border_lines
  attr_reader :cell_width
  attr_reader :cell_height
  attr_reader :cells_needing_updates

  def initialize(color_map, level_cfg, move_number_provider)
    @cells_needing_updates = []
    @move_number_provider = move_number_provider
    @gap_px = 10
    @single_rules = []
    @vlines = [make_line]
    @num_rows = 0
    @num_cols = 0
    @border_lines = [Line.new, Line.new, Line.new, Line.new]
    @hintbox = Rectangle.new(
      :color => "white",
      :z => 0,
      :opacity => 0.2)

    @hintbox.remove
    @border_lines.each {|line| line.remove}

    rules = level_cfg[LEVEL_RULES]
    rule_count = rules.size

    cell_factory = CellFactory.new(level_cfg, color_map, self)
    rules.each do |from, to|
      rule = SingleRule.new(self, color_map, from, to, cell_factory)
      @vlines << make_line
      @single_rules << rule
      @num_cols += rule.num_cols

      # num_rows in grid is 1 for "from", a "blank", and n replacements
      @num_rows = [@num_rows, rule.num_rows].max
    end

    @sep_hline = make_line
  end

  def cur_move_number
    # this is "lying" slightly, reporting the next move instead of cur move, so
    # the rules display that state that the cell _is going to be in_ if inserted now.
    # Purpose: For some cells that change based on turn number, what is shown
    # in the rule is the state it should be if it's played into the playarea row.
    # If the rule shows the current move state, then when a play occurs, the cell
    # may look different by showing the _next_ state.
    return @move_number_provider.cur_move_number + 1
  end

  def clear
    @single_rules.each do |rule|
      if rule == nil
        puts("************ RULE IS NIL")
      end
      rule.clear
    end
    @vlines.each {|line| line.remove}
    @sep_hline.remove
    unselect_hints
  end

  def unselect_hints
    @hintbox.remove
  end

  def resizing_move_to(x1, y1, x2, y2)
    @x1 = x1
    @y1 = y1
    @x2 = x2
    @y2 = y2

    width = (x1-x2).abs - (@gap_px * @single_rules.size)
    height = (y1-y2).abs
    @cell_width = width / @num_cols
    @cell_height = height / @num_rows

    x = x1 + @gap_px / 2
    rule_num = 0
    @single_rules.each do |rule|
      rule_width = @cell_width * rule.num_cols
      eff_y2 = y1 + @cell_height * rule.num_rows
      rule.resizing_move_to(x, y1, x + rule_width, eff_y2)
      update_vline(rule_num, x - @gap_px / 2, y1, y2)
      x += rule_width + gap_px
      rule_num += 1
    end
    update_vline(rule_num, x - @gap_px / 2, y1, y2)

    @sep_hline.x1 = x1
    @sep_hline.x2 = x2
    @sep_hline.y1 = y1 + @cell_height * 1.2
    @sep_hline.y2 = @sep_hline.y1

    x = x1
    rule_num = 0
    @vlines.each do |vline|
    end
  end

  def get_rule_for_pat_and_repl(raw_pat_str, raw_repl_str)
    @single_rules.each do |rule|
      if rule.from_str == raw_pat_str
        rule.replacement_strs.each do |repl|
          if repl == raw_repl_str
            return rule
          end
        end
      end
    end
    nil
  end

  def get_rule_at(x, y)
    @single_rules.each do |rule|
      if rule.x1 <= x && rule.x2 >= x && rule.y1 <= y && rule.y2 >= y
        return rule
      end
    end
    nil
  end

  # given x,y mouse coordinates, return the replacement under it
  # or nil if there isn't
  def get_replacement_str_at(x, y)
    rule = get_rule_at(x,y)
    if rule != nil
      row, _ = rule.rowcol_for_coord(x, y)
      return rule.get_replacement_str_for_row(row)
    end
  end

  private

  def update_vline(num, x, y1, y2)
    vline = @vlines[num]
    vline.x1 = x
    vline.y1 = y1
    vline.x2 = x
    vline.y2 = y2
  end

  def make_line
    Line.new(:z => 10, :color => 'white')
  end

end

