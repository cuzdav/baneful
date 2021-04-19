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

  def initialize(x, y, height, width, fgcolor = 'silver', bgcolor = 'black')
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
      Text.new('?', x: x + width / 2 - 0.28 * text_size, y: y, color: fgcolor, z: 20, size: text_size)
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

class WireframeRectangle
  attr_reader :x, :y, :z, :height, :width, :thickness, :color

  def initialize(opts = {})
    @x = opts[:x] || 0
    @y = opts[:y] || 0
    @z = opts[:z] || 10
    @width = opts[:width] || 200
    @height = opts[:height] || 100
    @color = opts[:color] || 'white'
    @thickness = opts[:thickness] || 1
    @topline = Rectangle.new(
      x: @x, y: @y, width: @width, height: @thickness, color: @color, z: @z)
    @bottomline = Rectangle.new(
      x: @x, y: @y + @height - @thickness, width: @width, height: @thickness, color: color, z: @z)
    @leftline = Rectangle.new(
      x: @x, y: @y, width: @thickness, height: @height, color: color, z: @z)
    @rightline = Rectangle.new(
      x: @x + @width - @thickness, y: @y, width: @thickness, height: @height, color: @color, z: @z)
  end

  def to_s
    "#WireframeRectangle<#{self}: x=#{@x}, y=#{@y}, z=#{@z}, width=#{@width}, height=#{@height}, " \
    "thickness=#{@thickness}, color=#{@color}>"
  end

  def add
    @topline.add
    @bottomline.add
    @leftline.add
    @rightline.add
  end

  def remove
    @topline.remove
    @bottomline.remove
    @leftline.remove
    @rightline.remove
  end

  def z=(z)
    @z = z
    @topline.z = z
    @bottomline.z = z
    @leftline.z = z
    @rightline.z = z
  end

  def color=(color)
    @color = color
    @topline.color = color
    @bottomline.color = color
    @leftline.color = color
    @rightline.color = color
  end

  def x=(x)
    @x = x
    dx = x - @topline.x
    @topline.x += dx
    @bottomline.x += dx
    @leftline.x += dx
    @rightline.x += dx
  end

  def y=(y)
    @y = y
    dy = y - @topline.y
    @topline.y += dy
    @bottomline.y += dy
    @leftline.y += dy
    @rightline.y += dy
  end

  def width=(w)
    @width = w
    dw = w - @topline.width
    @topline.width += dw
    @bottomline.width += dw
    @rightline.x += dw
  end

  def height=(h)
    @height = h
    dh = h - @leftline.height
    @leftline.height += dh
    @rightline.height += dh
    @bottomline.y += dh
  end
end

class AnimatedRectangleZoom

  # inputs are all optional, but thickness and linger cannot be changed after construction
  def initialize(opts = {})
    @x = opts[:x] || 0
    @y = opts[:y] || 0
    @z = opts[:z] || 0
    @width = opts[:width] || 200
    @height = opts[:height] || 100
    @color = opts[:color] || 'white'
    @thickness = opts[:thickness] || 1
    linger = opts[:linger] || 1
    @steps = opts[:steps] || 60
    @rects = []
    @idx = 0
    @count = 0
    @dx = 1
    @dy = 1
    @dx2 = 1
    @dy2 = 1
    linger.times do
      @rects << WireframeRectangle.new(x: @x, y: @y, z: @z, width: @width, height: @height, color: @color, thickness: @thickness)
    end
    @prevrect = @rects[0]
    @rects.each(&:remove)
  end

  def x=(x)
    @x = x
    @rects.each{ |rect| rect.x = x }
  end

  def y=(y)
    @y = y
    @rects.each{ |rect| rect.y = y }
  end

  def z=(z)
    @z = z
    @rects.each{ |rect| rect.z = z }
  end

  def height=(h)
    @height = h
    @rects.each{ |rect| rect.height = h }
  end

  def width=(w)
    @width = w
    @rects.each{ |rect| rect.width = w }
  end

  def color=(c)
    @color = c
    @rects.each{ |rect| rect.color = c }
  end

  def steps=(s)
    @steps = s
  end

  def add
    puts("**ADDING")
    @rects.each(&:add)
  end

  def remove
    puts("**REMOVING")
    @rects.each(&:remove)
  end

  def reset
    window_width = Window.get(:width)
    window_height = Window.get(:height)
    @rects.each do |rect|
      rect.x = 0
      rect.y = 0
      rect.width = window_width
      rect.height = window_height
      rect.add
    end
    @dx = 1.0 * @x / @steps
    @dy = 1.0 * @y / @steps
    @dx2 = 1.0 * (window_width - @width) / @steps
    @dy2 = 1.0 * (window_height - @height) / @steps
    @idx = 0
    @count = 0
    @prevrect = @rects[0]
    add
  end

  def update
    rect = @rects[@idx]
    @idx += 1
    linger = @rects.size
    @idx = 0 if @idx == linger
    rect.x = @prevrect.x + @dx
    rect.y = @prevrect.y + @dy
    rect.width = @prevrect.width - @dx2
    rect.height = @prevrect.height - @dy2
    @prevrect = rect
    @count += 1
    if rect.x >= @x
      remove
      return false
    end
    true
  end
end
