require 'ruby2d'

set title: "Hello Triangle"

r = Rectangle.new(
  x: 320, y:  50,
  width: 300,
  height: 80,
)

r.color = ['red', 'green', 'blue', 'white']

txt = Text.new("hello")
txt.size = 999
puts(txt.size)
txt.x=200

class Restart
  def initialize(x, y, radius, fgcolor='white', bgcolor='black')
    @circle = Circle.new(x:x, y:y, radius:radius, color:fgcolor)
    @black_circle = circle = Circle.new(x:x, y:y, radius:radius*0.8, color:bgcolor)
    @blackout_rect = Rectangle.new(x:x-radius, y:y-radius, width:radius, height:radius*2, color:bgcolor)
    @toprect = Rectangle.new(x:x-radius, y:y-radius, width:radius, height:radius*0.2, color: fgcolor)
    @botrect = Rectangle.new(x:x-radius, y:y+radius*0.8, width:radius, height:radius*0.2, color: fgcolor)

    arrowhead_pct=0.40
    y1 = y - radius - radius*arrowhead_pct
    y2 = y - radius * 0.80 + radius*arrowhead_pct
    @tri = Triangle.new(x1: x - radius,
                        x2: x - radius,
                        x3: x - radius - radius*0.50,
                        y1: y1,
                        y2: y2,
                        y3: (y2+y1)/2
                       )
  end
end

r = Restart.new(100, 300, 15)

show
