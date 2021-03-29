#require 'ruby2d'

class Rectangle
  def needs_modified_callback
    return false
  end
end

class CustomCellBase
  attr_accessor :x, :y, :width, :height

  def needs_modified_callback
    return true
  end

  def modified
    raise "'modified' must be overridden"
  end

  def each_wigit() raise("Not implemented") end

  def initialize()
    @x=0
    @y=0
    @z=10
    @width=0
    @height=0
  end

  def add()
    each_wigit do |wigit|
      wigit.add
    end
  end

  def remove()
    each_wigit do |wigit|
      wigit.remove
    end
  end

  def color=(color)
    each_wigit do |wigit|
      wigit.color = color
    end
  end

  def opacity=(opacity)
    each_wigit do |wigit|
      wigit.opacity=opacity
    end
  end

  def z=(val)
    each_wigit do |wigit|
      wigit.z = val
    end
  end

  def x=(val)
    @x = val
    modified
  end

  def y=(val)
    @y = val
    modified
  end

  def z=(val)
    @z = val
    modified
  end

  def width=(w)
    @width = w
    modified
  end

  def height=(h)
    @height = h
    modified
  end
end

# draw this for rules that are replaced with "nothing"
# Must implement Rect interface (aside from constructor)
# (Red circle with line through it)
class EmptyReplacementWigit < CustomCellBase
  def initialize(color)
    super()
    @circle = Circle.new(
      color: color,
      z: @z,
    )
    @line = Line.new(:z => @z)
  end

  def each_wigit(&block)
    block.yield @circle
    block.yield @line
  end

  def modified()
    min_hw = [@height, @width].min / 2 - 1
    mid_x = @x + @width / 2
    mid_y = @y + @height / 2
    mid_z = @z
    @circle.x = mid_x
    @circle.y = mid_y
    @circle.z = @z
    @circle.radius = min_hw

    @line.x1 = mid_x + min_hw
    @line.y1 = mid_y - min_hw
    @line.x2 = mid_x - min_hw
    @line.y2 = mid_y + min_hw
    @line.z = @z
  end
end

class WildcardWigit < CustomCellBase

  #wc_num is (wildcard) number 1-9 or nil
  def initialize(wc_num)
    super()
    @rect = Rectangle.new(color: get_color())
    @circ = Circle.new(color: "red")
    @circ2 = Circle.new(color: "black")
    @text_data = wc_num ? idx : '?'
    @text = Text.new(@text_data)
  end

  def get_color()
      return ['red', 'blue', 'lime', 'yellow']
  end

  def color=(color)
  end

  def each_wigit(&block)
    block.yield @rect
    block.yield @text
    block.yield @circ
    block.yield @circ2
  end

  def modified()
    @rect.height = height
    @rect.width = width
    @rect.x = @x
    @rect.y = @y
    @rect.z = @z

    @circ.x = @x + @width / 2
    @circ.y = @y + @height / 2
    @circ.z = @z
    @circ.radius = @height / 2
    @circ2.x = @x + @width / 2
    @circ2.y = @y + @height / 2
    @circ2.z = @z
    @circ2.radius = @height / 2 - 3

    @text.remove if @text
    @text = Text.new(
      @text_data,
      size:@height * 0.80,
      x:@x + @width / 2 - 11,
      y:@y + 2,
      z:@z,
    )
    @text.size = 100
  end
end


#
# Receives an index source, which when queried will state which index
# should be drawn (externally modifiedd)
# Also, passed 2..3 colors, that it will cycle through
#
# Requires: move_number_provider: any class that has a "cur_move_number" method
# that returns a number

class RotatingColorsWigit < CustomCellBase

  def initialize(move_number_provider, *colors)
    super()
    if colors.size < 2 or colors.size > 3
      raise "Colors should be 2..3 provided.  Got #{colors.size}"
    end

    @move_number_provider = move_number_provider
    @colors = colors.dup

    puts("*** RotatingColorsWigit colors: #{@colors}")

    @circle_colors  = colors.map{ |color| Circle.new(color: color) }
    @outline_colors = colors.map{ |color| Circle.new(color: 'black') }
    @rect = Rectangle.new(z: @z)
    @n = @outline_colors.size
    modified
  end

  def get_cur_index()
    return @move_number_provider.cur_move_number() % @n
  end

  def color=(color)
    # ignore
  end

  def color()
    return @rect.color
  end

  def each_wigit(&block)
    @circle_colors.each do |circle|
      block.yield circle
    end
    @outline_colors.each do |circle|
      block.yield circle
    end
    block.yield @rect
  end

  def modified()
    idx = get_cur_index()
    @rect.height = @height
    @rect.width = @width
    @rect.x = @x
    @rect.y = @y
    @rect.z = @z
    @rect.color = @colors[idx]

    dx = @width / (@n+1)
    x = @x + dx
    (0...@n).each do |i|
      outline = @outline_colors[i]
      outline.x = x
      outline.y = @y + (@height / 2)
      outline.radius = height / 2 - 6

      circle = @circle_colors[i]
      circle.x = x
      circle.y = @y + (@height / 2)
      circle.color = @colors[i]
      circle.radius = height / 2 - 10

      zd = i == idx ? 10 : 0
      outline.color = i == idx ? 'white' : 'black'

      z = @z + @n + zd + (i < idx ? +i : -i)
      outline.z = z
      circle.z = z
      x += dx
    end
  end
end
