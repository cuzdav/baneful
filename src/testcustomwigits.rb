require 'ruby2d'
require "test/unit"
require_relative "custom_cells.rb"

module ExtraColorMethod
  def ==(other)
    raise "only compare to Color objects" if other.class != Color

    return (self.a == other.a and
      self.r == other.r and
      self.g == other.g and
      self.b == other.b)
  end
end

Color.class_eval { include ExtraColorMethod }

RED = Color.new('red')
BLUE = Color.new('blue')
GREEN = Color.new('green')


class TestCustomWidgets < Test::Unit::TestCase

  @move_number = 0

  def setup()
    @move_number = 0
    @rot_rb = RotatingColorsWidget.new(self, 'red', 'blue')
    @rot_rbg = RotatingColorsWidget.new(self, 'red', 'blue', 'green')
    @rot_rbb = RotatingColorsWidget.new(self, 'red', 'blue', 'blue')
  end

  def cur_move_number
    return @move_number
  end

  def next_turn
    @move_number += 1
    @rot_rb.update
    @rot_rbg.update
    @rot_rbb.update
  end

  def test_rotating_colors()

    assert_true(@rot_rb.instance_variable_get("needs_update"))

    assert_equal(RED, @rot_rb.color)
    assert_equal(RED, @rot_rbg.color)
    assert_equal(RED, @rot_rbb.color)

    next_turn
    assert_equal(BLUE, @rot_rb.color)
    assert_equal(BLUE, @rot_rbg.color)
    assert_equal(BLUE, @rot_rbb.color)

    next_turn
    assert_equal(RED, @rot_rb.color)
    assert_equal(GREEN, @rot_rbg.color)
    assert_equal(BLUE, @rot_rbb.color)

    next_turn
    assert_equal(BLUE, @rot_rb.color)
    assert_equal(RED, @rot_rbg.color)
    assert_equal(RED, @rot_rbb.color)

    next_turn
    assert_equal(RED, @rot_rb.color)
    assert_equal(BLUE, @rot_rbg.color)
    assert_equal(BLUE, @rot_rbb.color)

    next_turn
    assert_equal(BLUE, @rot_rb.color)
    assert_equal(GREEN, @rot_rbg.color)
    assert_equal(BLUE, @rot_rbb.color)
  end

end
