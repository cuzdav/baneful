require 'ruby2d'
require_relative('../extra_widgets')
set title: "Hello Triangle"

r = Rectangle.new(
  x: 320, y: 50,
  width: 300,
  height: 80,
)

r.color = ['red', 'green', 'blue', 'white']

txt = Text.new("hello")
txt.size = 999
puts(txt.size)
txt.x = 200


r = UndoArrow.new(100, 300,50)
r.add

q = QuestionMark.new(200, 200, 30,30)
q.add

Window.on :mouse_up do |event|
  if event.button == :left
    if q.contains?(event.x, event.y)
      puts("YES")
    else
      puts("NO")
    end
    puts("Mouse: (#{event.x},#{event.y}), rect=(#{r.x},#{r.y})-(#{r.x+r.width},#{r.y+r.height})")
  end
  p event
end


show
