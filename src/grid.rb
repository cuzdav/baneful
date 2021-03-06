# frozen_string_literal: true
require 'ruby2d'


class Grid
  attr_reader :gap_px, :vert_gap_px, :cell_width,
              :cell_height, :num_rows, :num_cols, :x1, :x2, :y1, :y2

  def initialize(num_rows, num_cols, x1, y1, x2, y2)
    @num_rows = num_rows
    @num_cols = num_cols
    @rows = []
    @row_xoffsets = []
    @gap_px = 2
    @vert_gap_px = 5
    @background = Rectangle.new(z: 0, color: [1, 1, 1, 0.4])
    @background.remove

    (0...@num_rows).each do|_|
      @rows << []
      @row_xoffsets << 0
    end

    resizing_move_to(x1, y1, x2, y2)

    @selectionbox = [
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20)
    ]
    @selectionbox.each(&:remove)
    @selected = []
  end

  # One use is for centering rows out-of-phase rows to grid width
  def set_row_offset(row, offset)
    @row_xoffsets[row] = offset
  end

  def set_background_color(color)
    @background.color = color
  end

  def highlight_background
    @background.add
  end

  def unhighlight_background
    @background.remove
  end

  # allow some other object to replace a rect, as long as it implements
  # the Rectangle interface
  def set_cell_object(row, col, obj)
    cell = @rows[row][col]
    cell&.remove
    @rows[row][col] = obj
    update_cell(obj, row, col, nil)
  end

  def get_cell_object(row, col)
    @rows[row][col]
  end

  def refresh
    resizing_move_to(@x1, @y1, @x2, @y2)
  end

  # move grid and all contents
  def resizing_move_to(x1, y1, x2, y2, color = nil)
    @x1 = x1.to_i
    @y1 = y1.to_i
    @x2 = x2.to_i
    @y2 = y2.to_i

    @background.x = x1
    @background.y = y1
    @background.height = y2 - y1
    @background.width  = x2 - x1

    @cell_width  = (x2 - x1).abs / @num_cols
    @cell_height = (y2 - y1).abs / @num_rows

    foreach_rect_with_index do |rect, row, col|
      update_cell(rect, row, col, color)
    end

    select_cells(@selected_row, @selected_col1, @selected_col2) unless @selected_row.nil?
  end

  def foreach_rect(&block)
    (0...@num_rows).each do |r|
      (0...@num_cols).each do |c|
        cell = @rows[r][c]
        block.call(cell) if cell
      end
    end
  end

  def foreach_rect_with_index(&block)
    (0...@num_rows).each do |r|
      (0...@num_cols).each do |c|
        cell = @rows[r][c]
        block.call(cell, r, c) if cell
      end
    end
  end

  def clear
    foreach_rect(&:remove)
    unhighlight_background
    unselect
  end

  def rowcol_for_coord(x, y)
    row = (y >= @y1) && (y < @y2) ? (y - @y1) / @cell_height : nil
    col = (x >= @x1) && (x < @x2) ? (x - @x1) / @cell_width : nil
    [row, col]
  end

  def xcoord(rownum, colnum)
    (colnum.floor * @cell_width + @x1).floor + @row_xoffsets[rownum]
  end

  def ycoord(rownum)
    (rownum.floor * @cell_height + @y1).floor
  end

  def set_cell_color(rownum, colnum, color)
    cell = @rows[rownum][colnum]
    cell.color = color
    cell
  end

  def set_cell_opacity(rownum, colnum, opacity)
    if rownum < num_rows
      cell = @rows[rownum][colnum]
      cell.opacity = opacity if cell
      return cell
    end
    nil
  end

  def show_cell(rownum, colnum)
    if rownum < num_rows
      cell = @rows[rownum][colnum]
      cell&.add
      return cell
    end
    nil
  end

  def hide_cell(rownum, colnum)
    if rownum < num_rows
      cell = @rows[rownum][colnum]
      cell&.remove
      return cell
    end
    nil
  end

  def select_row(rownum, color = 'yellow', width = 5)
    select_cells(rownum, 0, @num_cols-1, color, width) if rownum < @num_rows
  end

  def selected_coords
    @selected_coords
  end

  def select_cells(rownum, colnum1, colnum2, color = 'yellow', width = 5)
    @selected_row = rownum
    @selected_col1 = colnum1
    @selected_col2 = colnum2

    x = xcoord(rownum, colnum1) + @gap_px
    x2 = xcoord(rownum, colnum2) + @cell_width
    y = ycoord(rownum) + @vert_gap_px
    y2 = y + @cell_height - @vert_gap_px
    @selected_coords = [x, y, x2, y2]

    # top horiz
    @selectionbox[0].x1 = x
    @selectionbox[0].y1 = y
    @selectionbox[0].x2 = x2
    @selectionbox[0].y2 = y
    @selectionbox[0].add

    # bottom horiz
    @selectionbox[1].x1 = x
    @selectionbox[1].y1 = y2
    @selectionbox[1].x2 = x2
    @selectionbox[1].y2 = y2
    @selectionbox[1].add

    # left vert
    @selectionbox[2].x1 = x
    @selectionbox[2].y1 = y
    @selectionbox[2].x2 = x
    @selectionbox[2].y2 = y2
    @selectionbox[2].add

    # right vert
    @selectionbox[3].x1 = x2
    @selectionbox[3].y1 = y
    @selectionbox[3].x2 = x2
    @selectionbox[3].y2 = y2
    @selectionbox[3].add

    @selectionbox.each do |box|
      box.color = color
      box.width = width
    end
  end

  def unselect
    @selectionbox.each(&:remove)
    @selected_row = nil
    @selected_col1 = nil
    @selected_col2 = nil
    @selection_coords = nil
  end

  private

  def update_cell(rect, row, col, color)
    return if rect.nil?

    rect.x = xcoord(row, col) + @gap_px
    rect.width = @cell_width - @gap_px
    rect.y = ycoord(row) + @vert_gap_px
    rect.height = @cell_height - @vert_gap_px
    rect.color = color unless color.nil?
  end
end
