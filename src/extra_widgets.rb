require 'ruby2d'

class UndoArrow
  attr_reader :width, :height, :x, :y

  def initialize(x, y, width, fgcolor = 'silver', bgcolor = 'black')
    radius = width / 2
    x += radius
    y += radius
    arrowhead_pct = 0.40
    arr_y1 = y - radius - radius * arrowhead_pct
    arr_y2 = y - radius * 0.80 + radius * arrowhead_pct
    half_rad = radius * 0.5
    @x = x - radius
    @y = arr_y1
    @width = radius * 2
    @height = radius * 2 + radius * arrowhead_pct

    @parts = [
      # outer circle
      Circle.new(x: x, y: y, radius: radius, color: fgcolor),
      # inner "empty" circle
      Circle.new(x: x, y: y, radius: radius * 0.8, color: bgcolor),

      # "empty rectangle"
      Rectangle.new(x: @x, y: @y, width: radius, height: @height, color: bgcolor),

      # small top rect / thick line
      Rectangle.new(x: x - half_rad, y: y - radius, width: half_rad, height: radius * 0.2, color: fgcolor),

      # small bottom rect / thick line
      Rectangle.new(x: x - radius, y: y + radius * 0.8, width: radius, height: radius * 0.2, color: fgcolor),

      # arrowhead
      Triangle.new(x1: @x + half_rad,
                   x2: @x + half_rad,
                   x3: @x,
                   y1: arr_y1,
                   y2: arr_y2,
                   y3: (arr_y2 + arr_y1) / 2
      )
    ]
    @parts.each { |part| part.z = 20 }
    remove
  end

  def add
    @parts.each(&:add)
  end

  def remove
    @parts.each(&:remove)
  end

  def contains?(x, y)
    (x >= @x && x <= @x + @width) && (y >= @y && y <= @y + @height)
  end
end

class QuestionMark
  attr_reader :x, :y, :height, :width

  def initialize(x, y, height, width, fgcolor='silver', bgcolor='black')
    @x = x
    @y = y
    @width = width
    @height = height

    thickness = 2
    text_size = height - 2 * thickness
    @parts = [
      # visible bounding box
      Rectangle.new(x: x, y: y, height: height, width: width, z: 20, color: fgcolor),

      # hollowed out center box
      Rectangle.new(x: x + thickness,
                    y: y + thickness,
                    height: height - 2 * thickness,
                    width: width - 2 * thickness,
                    z: 20,
                    color: bgcolor
      ),

      # actual question mark
      Text.new('?', x: x + width / 2 - 0.28 * text_size , y: y, color: fgcolor, z: 20, size: text_size)
    ]
    remove
  end

  def add
    @parts.each(&:add)
  end

  def remove
    @parts.each(&:remove)
  end

  def contains?(x, y)
    @parts[0].contains?(x, y)
  end
end
