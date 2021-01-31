require 'ruby2d'


class Grid
  attr_reader :gap_px
  attr_reader :vert_gap_px
  attr_reader :cell_width
  attr_reader :cell_height
  attr_reader :num_rows
  attr_reader :num_cols
  attr_reader :x1
  attr_reader :x2
  attr_reader :y1
  attr_reader :y2

  def initialize(num_rows, num_cols, x1, y1, x2, y2)
    @num_rows = num_rows
    @num_cols = num_cols
    @rows = []
    @gap_px = 2
    @vert_gap_px = 5
    @background = Rectangle.new(:z => 1, :color => [1, 1, 1, 0.4])
    @background.remove

    (0...@num_rows).each do|y|
      @rows << []
      (0...@num_cols).each do |x|
        @rows[y] << Rectangle.new(:z => 10)
      end
    end

    resizing_move_to(x1, y1, x2, y2)

    @selectionbox = [
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20),
      Line.new(color: 'yellow', z: 20)
    ]
    @selectionbox.each do |s|
      s.remove
    end
    @selected = []
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
    @rows[row][col].remove
    @rows[row][col] = obj
  end

  # move grid and all contents
  def resizing_move_to(x1, y1, x2, y2, color=nil)
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
      rect.x = xcoord(col) + @gap_px
      rect.width = @cell_width - @gap_px
      rect.y = ycoord(row) + @vert_gap_px
      rect.height = @cell_height - @vert_gap_px
      rect.color = color if color != nil
    end

    if @selected_row != nil
      select_cells(@selected_row, @selected_col1, @selected_col2)
    end
  end

  def foreach_rect(&block)
    (0...@num_rows).each do |r|
      (0...@num_cols).each do |c|
        block.call(@rows[r][c])
      end
    end
  end

  def foreach_rect_with_index(&block)
    (0...@num_rows).each do |r|
      (0...@num_cols).each do |c|
        block.call(@rows[r][c], r, c)
      end
    end
  end

  def remove_all()
    foreach_rect do |rect|
      rect.remove
    end
  end

  def rowcol_for_coord(x, y)
    row = (y >= @y1 and y < @y2) ? (y - @y1) / @cell_height : nil
    col = (x >= @x1 and x < @x2) ? (x - @x1) / @cell_width : nil

    result = row, col
    return result
  end

  def xcoord(col)
    (col.floor * @cell_width + @x1).floor
  end

  def ycoord(row)
    (row.floor * @cell_height + @y1).floor
  end

  def set_cell_color(row, col, color)
    cell = @rows[row][col]
    cell.color = color
    return cell
  end

  def set_cell_opacity(row, col, opacity)
    cell = @rows[row][col]
    cell.opacity = opacity
    return cell
  end

  def show_cell(row, col)
    @rows[row][col].add
  end

  def hide_cell(row, col)
    @rows[row][col].remove
  end

  def select_row(row, color='yellow', width=5)
    select_cells(row, 0, @num_cols-1, color, width)
  end

  def select_cells(row, col1, col2, color='yellow', width=5)
    @selected_row = row
    @selected_col1 = col1
    @selected_col2 = col2

    x = xcoord(col1) + @gap_px
    x2 = xcoord(col2) 
    y = ycoord(row) + @vert_gap_px

    # top horiz
    @selectionbox[0].x1 = x
    @selectionbox[0].y1 = y
    @selectionbox[0].x2 = x2 + @cell_width
    @selectionbox[0].y2 = y
    @selectionbox[0].add

    # bottom horiz
    @selectionbox[1].x1 = x
    @selectionbox[1].y1 = y + @cell_height - @vert_gap_px
    @selectionbox[1].x2 = x2 + @cell_width
    @selectionbox[1].y2 = y + @cell_height - @vert_gap_px
    @selectionbox[1].add

    # left vert
    @selectionbox[2].x1 = x
    @selectionbox[2].y1 = y
    @selectionbox[2].x2 = x
    @selectionbox[2].y2 = y + @cell_height - @vert_gap_px
    @selectionbox[2].add

    # right vert
    @selectionbox[3].x1 = x2 + @cell_width
    @selectionbox[3].y1 = y
    @selectionbox[3].x2 = x2 + @cell_width
    @selectionbox[3].y2 = y + @cell_height - @vert_gap_px
    @selectionbox[3].add

    @selectionbox.each do |box|
      box.color = color
      box.width = width
    end
  end

  def unselect()
    @selectionbox.each {|s| s.remove}
    @selected_row = nil
    @selected_col1 = nil
    @selected_col2 = nil
  end
end
