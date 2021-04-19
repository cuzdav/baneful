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

r = UndoArrow.new(100, 300, 50)
r.add

q = QuestionMark.new(200, 200, 30, 30)
q.add


def qclicked(event)
  if event.button == :left
    if q.contains?(event.x, event.y)
      puts("YES")
    else
      puts("NO")
    end
    puts("Mouse: (#{event.x},#{event.y}), rect=(#{r.x},#{r.y})-(#{r.x + r.width},#{r.y + r.height})")
  end
end

def interact_with_wireframe(event)
  case event.key
  when "up"
    case var
    when 'x'
      puts("before #{w.x}")
      w.x += 10
      puts("after #{w.x}")
    when 'y'
      w.y += 10
    when 'h'
      w.height += 10
    when 'w'
      w.width += 10
    end

  when "down"
    case var
    when 'x'
      w.x -= 10
    when 'y'
      w.y -= 10
    when 'h'
      w.height -= 10
    when 'w'
      w.width -= 10
    end

  when /[xyhw]/
    var = event.key
    puts var
  end
end

animated = AnimatedRectangleZoom.new(x: 50, y: 100, width: 40, height: 10, color: 'yellow', linger: 4, steps: 40)

animating = false
Window.on :mouse_up do |event|
  animated.reset
  animating = true
end

# # w = WireframeRectangle.new(x:50, y:100, width: 50, height:35, thickness:1, color:'white')
# w.add

var = "x"
Window.on :key_up do |event|
end

t = 0
update do
  t += 1
  if true # t % 5 == 0
    if animating
      if !animated.update
        animating = false
      end
    end
  end
end
show
