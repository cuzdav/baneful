require 'ruby2d'
require_relative 'custom_wigits.rb'

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
  def initialize(parent, color_map, from_str, replacement_strs)
    @parent = parent
    @from_str = from_str
    @replacement_strs = replacement_strs
    @num_replacements = replacement_strs.size
    @num_rows = @num_replacements + FIRST_REPL_ROW
    @num_cols = [from_str.size, *replacement_strs.map {|str| str.size}].max
    @rule_grid = Grid.new(@num_replacements + FIRST_REPL_ROW, @num_cols, 0,0,0,0)
    @rule_grid.remove_all
    @color_map = color_map

    # add colors for rules and replacements
    set_rule_source_cells(from_str)
    set_replacement_cells(replacement_strs)
  end


  def clear
    @rule_grid.remove_all
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
    @replacement_strs.each do |repl|
      return idx if repl == repl_str
      idx += 1
    end
    nil
  end

  def show_pattern_hint
    pat_hint = get_hintbox
    pat_hint.y = @rule_grid.y1
    pat_hint.add
  end

  # row: 0 based, in terms of config list, not grid offset
  def show_repl_hint(repl_str)
    idx = index_of_repl(repl_str)
    return if idx == nil

    repl_hint = get_hintbox
    repl_hint.y = @rule_grid.ycoord(idx + 2)
    repl_hint.add
  end

  def unselect_hints
    @parent.unselect_hints
  end

  private

  # given a string pattern, enable and colorize the cells in the grid
  def set_rule_source_cells(from_str)
    row = 0
    col = 0
    from_str.each_char do |ch|
      if ch == '.'
        @rule_grid.set_cell_object(row, col, WildcardWigit.new)
      else
        @rule_grid.set_cell_color(row, col, @color_map[ch])
      end

      @rule_grid.show_cell(row, col)
      col += 1
    end
  end

  def set_replacement_cells(replacement_strs)
    row = 2
    col = 0
    replacement_strs.each do |repl_str|
      if not repl_str.empty?
        repl_str.each_char do |ch|
          if SPECIAL_REPL_CHARS.include?(ch)
            @rule_grid.set_cell_object(row, col, WildcardWigit.new)
          else
            @rule_grid.set_cell_color(row, col, @color_map[ch])
          end
          @rule_grid.show_cell(row, col)
          col += 1
        end
      else
        empty = EmptyReplacementWigit.new('red')
        @rule_grid.set_cell_object(row, col, empty)
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
    return hintbox
  end


end


#
# Collection and manager of SingleRule objects
#
class RuleUI
  attr_accessor :gap_px
  attr_accessor :rules
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

  def initialize(color_map, rules)
    @gap_px = 10
    @rules = []
    @vlines = [make_line]
    @num_rows = 0
    @num_cols = 0
    @border_lines = [Line.new, Line.new, Line.new, Line.new]
    @hintbox = Rectangle.new(
      :color => "white",
      :z => 0,
      :opacity => 0.8)

    @hintbox.remove
    @border_lines.each {|line| line.remove}

    rule_count = rules.size

    rules.each do |from, to|
      rule = SingleRule.new(self, color_map, from, to)
      @vlines << make_line
      @rules << rule
      @num_cols += rule.num_cols

      # num_rows in grid is 1 for "from", a "blank", and n replacements
      @num_rows = [@num_rows, rule.num_rows].max
    end

    @sep_hline = make_line
  end

  def clear
    @rules.each do |rule|
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

    width = (x1-x2).abs - (@gap_px * @rules.size)
    height = (y1-y2).abs
    @cell_width = width / @num_cols
    @cell_height = height / @num_rows

    x = x1 + @gap_px / 2
    rule_num = 0
    @rules.each do |rule|
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

  # find a rule that can produce the given move, matching the from (pat)
  # and containing the replace pattern.
  # The move provided is fully resolved, so matching it to a rule requires
  # accounting for wildcards in the rule code.
  def get_rule_matching_move(move)
    resolved_from_str = move[GS_PLAY_PAT]
    resolved_to_str = move[GS_PLAY_REPL]
    @rules.each do |rule|
      idx, captures = wc_index(resolved_from_str, rule.from_str, 0)
      if idx != nil and rule.from_str.size == resolved_from_str.size
        # rule pattern matches, but does a repl also match?  Given
        # wildcards, it's possible this rule matches the from_str pat,
        # but has no replacement that matches.
        rule.replacement_strs.each do |repl|
          if wc_with_placeholder_equals(resolved_to_str, repl, captures)
            return rule
          end
        end
      end
    end
    return nil
  end

  def get_rule_for_string(from_str)
    @rules.each do |rule|
      if rule.from_str == from_str
        return rule
      end
    end
    nil
  end

  def get_rule_at(x, y)
    @rules.each do |rule|
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

