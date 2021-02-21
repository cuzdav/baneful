require "test/unit"
require_relative "solve2.rb"

class TestSolver < Test::Unit::TestCase

  def setup()
    @rules = {
        "ab"  => ["bb"],
        "ccc" => [""],
        "bc"  => ["c"],
        "d"   => ["dd"]
    }
    @max_width = 7
    @max_moves = 4
  end


  def assert_solution(rules, init_position, expected_solution)
    solver = Solver.new(rules, [init_position], @max_width, @max_moves)
    solution = solver.find_solution(init_position)

    if solution != nil
      # convert hash back into a small array for comparison
      model_solution = solution.map do |move|
        [move[GS_PLAY_IDX], move[GS_PLAY_PAT], move[GS_PLAY_REPL]]
      end
    else
      model_solution = nil
    end

    if (expected_solution != model_solution)
      puts("FAILURE: Here are known partial solutions:")
      p @visited
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
        "bc"  => ["c", "d"],
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

  def test_solver4()
    @max_moves = 10
    assert_solution(
      {
        "a"    => ["ba", "bb"],
        "ba"   => ["ab"],
        "abbbb" => [""]
      },
      "a",
      [[0, "a", "ba"],
       [0, "ba", "ab"],
       [0, "a", "ba"],
       [0, "ba", "ab"],
       [0, "a", "ba"],
       [1, "a", "ba"],
       [1, "ba", "ab"],
       [0, "ba", "ab"],
       [0, "abbbb", ""],
      ]
    )
  end


end

