require "test/unit"
require_relative "solve.rb"

class TestSolver < Test::Unit::TestCase

  def setup()
    @state = GameState.new(
      {
        "ab"  => ["bb"],
        "ccc" => [""],
        "bc"  => ["c"],
        "d"   => ["dd"]
      },
      "", # initial_str
      10, # num_moves
      10  # max_width of board
    )
  end


  def assert_solution(rules, init_position, expected_solution)
    state = GameState.new(rules, init_position, 10)
    solution = Solver.new().find_solution(state)

    # convert hash back into a small array for comparison
    model_solution = solution.map do |move|
      [move[GS_PLAY_IDX], move[GS_PLAY_PAT], move[GS_PLAY_REPL]]
    end
    assert_equal(expected_solution, model_solution)
  end

  def test_solver1()
    assert_solution(
      {
        "ab"  => ["bb"],
        "ccc" => [""],
        "bc"  => ["c"],
      },
      "abccc",
      [
        [0, "ab", "bb"],
        [1, "bc", "c"],
        [0, "bc", "c"],
        [0, "ccc", ""]
      ]
    )
  end

  def test_solver2()
    assert_solution(
      {
        "ab"  => ["ab", "bb"],
        "ccc" => [""],
        "bc"  => ["c"],
        "d"   => ["dd"]
      },
      "abccc",
      [
        [0, "ab", "bb"],
        [1, "bc", "c"],
        [0, "bc", "c"],
        [0, "ccc", ""]
      ]
    )
  end

  def test_solver3()
    assert_solution(
      {
        "ab"    => ["ab", "bb"],
        "ccc"   => [""],
        "bc"    => ["c"],
        "d"     => ["dd"],
        "abccc" => [""]
      },
      "abccc",
      [
        [0, "abccc", ""],
      ]
    )
  end


end

