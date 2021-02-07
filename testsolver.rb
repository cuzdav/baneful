require "test/unit"
require_relative "solve.rb"

class TestSolver < Test::Unit::TestCase

  def setup()
    @state = GameState.new(
      {
        "=*"  => ["**"],
        "---" => [""],
        "*-"  => ["-"],
        "+"   => ["++"]
      },
      "", # initial_str
      10, # num_moves
      10  # max_width of board
    )
  end


  def assert_solution(rules, init_position, expected_solution)
    state = GameState.new(rules, init_position, 10)
    solution = Solver.new().find_solution(state)
    assert_equal(expected_solution, solution)
  end

  def test_solver1()
    assert_solution(
      {
        "=*"  => ["**"],
        "---" => [""],
        "*-"  => ["-"],
      },
      "=*---",
      [
        [0, "=*", "**"],
        [1, "*-", "-"],
        [0, "*-", "-"],
        [0, "---", ""]
      ]
    )
  end

  def test_solver2()
    assert_solution(
      {
        "=*"  => ["=*", "**"],
        "---" => [""],
        "*-"  => ["-"],
        "+"   => ["++"]
      },
      "=*---",
      [
        [0, "=*", "**"],
        [1, "*-", "-"],
        [0, "*-", "-"],
        [0, "---", ""]
      ]
    )
  end

  def test_solver3()
    assert_solution(
      {
        "=*"    => ["=*", "**"],
        "---"   => [""],
        "*-"    => ["-"],
        "+"     => ["++"],
        "=*---" => [""]
      },
      "=*---",
      [
        [0, "=*---", ""],
      ]
    )
  end


end

