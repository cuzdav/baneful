require 'ruby2d'

class GameOverState
  def initialize
    @text = Text.new(
      "No More Moves",
      x: 50,
      y: playarea_height - 150,
      size: 80,
      color: "red",
      z: 10
    )
    @text2 = Text.new(
      "click to restart",
      x: 100,
      y: playarea_height - 100,
      size: 30,
      color: "white",
      z: 10
    )
  end

  def clear()
    @text.remove
    @text2.remove
  end
end
