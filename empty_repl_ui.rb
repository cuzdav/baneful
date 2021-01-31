#require 'ruby2d'

# draw this for rules that are replaced with "nothing"
# Must implement Rect interface (aside from constructor)
class EmptyReplacementWigit
  attr_reader :x, :y, :width, :height

  def initialize(color)
    @x=0
    @y=0
    @width=0
    @height=0

    @circle = Circle.new(
      color: color,
      z: 10,
    )
    @line = Line.new(:z => 10)
  end

  def add()
    @circle.add
    @line.add
  end

  def remove()
    @circle.remove
    @line.remove
  end

  def x=(x)
    @x = x
    update
  end

  def y=(y)
    @y = y
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

  def color=(color)
    @circle.color = color
    @line.color = color
  end

  def opacity=(opacity)
    @circle.opacity = opacity
    @line.opacity = opacity
  end

  def z=(z)
    @circle.z = z
    @line.z = z
  end

  private

  def update()
    min_hw = [@height, @width].min / 2 - 1
    mid_x = @x + @width / 2
    mid_y = @y + @height / 2
    @circle.x = mid_x
    @circle.y = mid_y
    @circle.radius = min_hw 

    puts("*** empty repl: min_nw = #{min_hw}, h:#{@height}, w:#{@width}")

    @line.x1 = mid_x + min_hw
    @line.y1 = mid_y - min_hw
    @line.x2 = mid_x - min_hw
    @line.y2 = mid_y + min_hw
  end

end
