require 'ruby2d'

class GameOverState
  def initialize
    @text = Text.new(
      "No More Moves",
      x: 50,
      y: playarea_height - 30,
      size: 80,
      color: "red",
      z: 10
    )
    @text2 = Text.new(
      "'r' (or click) - restart",
      x: 100,
      y: playarea_height + 50,
      size: 30,
      color: "white",
      z: 10
    )
    @text3 = Text.new(
      "'u' - undo last move",
      x: 100,
      y: playarea_height + 85,
      size: 30,
      color: "white",
      z: 10
    )
  end

  def clear()
    @text.remove
    @text2.remove
    @text3.remove
  end
end
