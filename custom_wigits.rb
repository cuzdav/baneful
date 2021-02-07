#require 'ruby2d'

class CustomCellBase
  attr_accessor :x, :y, :width, :height

  def update() raise("Not implemented") end
  def each_wigit() raise("Not implemented") end

  def initialize()
    @x=0
    @y=0
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
    update
  end

  def y=(val)
    @y = val
    update
  end

  def width=(w)
    @width = w
    update
  end

  def height=(h)
    @height = h
    update
  end
end

# draw this for rules that are replaced with "nothing"
# Must implement Rect interface (aside from constructor)
class EmptyReplacementWigit < CustomCellBase
  def initialize(color)
    super()
    @circle = Circle.new(
      color: color,
      z: 10,
    )
    @line = Line.new(:z => 10)
  end

  def each_wigit(&block)
    block.yield @circle
    block.yield @line
  end

  def update()
    min_hw = [@height, @width].min / 2 - 1
    mid_x = @x + @width / 2
    mid_y = @y + @height / 2
    @circle.x = mid_x
    @circle.y = mid_y
    @circle.radius = min_hw

    @line.x1 = mid_x + min_hw
    @line.y1 = mid_y - min_hw
    @line.x2 = mid_x - min_hw
    @line.y2 = mid_y + min_hw
  end
end

class WildcardWigit < CustomCellBase

  def initialize()
    super()
    @rects = []
    COLORS.each do |color|
      @rects << Rectangle.new(color: color)
    end
  end

  def color=(color)
  end

  def each_wigit(&block)
    @rects.each do |rect|
      block.yield rect
    end
  end

  def update()
    wdelta = width / @rects.size
    idx = 0
    @rects.each do |rect|
      rect.height = @height
      rect.width = wdelta
      rect.x = @x + idx * wdelta
      rect.y = @y
      idx += 1
    end
  end
end
