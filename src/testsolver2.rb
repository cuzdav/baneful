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

  def validate_solution(game_data, solution)
    return if solution.empty?
    begin
      solution.each do |move|
        game_data.make_move(move)
      end
      assert_true game_data.solved?
    ensure
      game_data.reset
    end
  end

  def assert_solution(rules, init_position, expected_solution, type_overrides=nil)
    solver = Solver.new(rules, @max_moves, @max_width)
    game_data = GameState.new(rules, init_position, type_overrides, @max_width)
    validate_solution(game_data, expected_solution)

    solution = solver.find_solution(game_data)
    validate_solution(game_data, solution)

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

  def test_constructor
    rules = {"ccc" => ["bbb"]}
    moves = 99
    width = 8
    solver = Solver.new(rules, moves, width)

    assert_equal(create_reverse_mapping(rules), solver.rev_rules)
    assert_equal(moves, solver.max_moves)
    assert_equal(width, solver.max_width)
  end

  def test_solver1()
    puts("solver1: width #{@max_width}")
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
    puts("solver2: width #{@max_width}")
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
      [
        [0, "a", "ba"],
        [0, "ba", "ab"],
        [0, "a", "ba"],
        [0, "ba", "ab"],
        [0, "a", "ba"],
        [1, "a", "ba"],
        [1, "ba", "ab"],
        [0, "ba", "ab"],
        [0, "abbbb", ""]
      ]
    )
  end

  def test_solver_wildcard1()
    @max_moves = 10
    assert_solution(
      {
        "."    => [""],
      },
      "ab",
      [[0, "a", ""],
       [0, "b", ""],
      ]
    )
  end

  def test_solver_wildcard2()
    @max_moves = 10
    assert_solution(
      {
        "b"      => [""],
        "a.a"    => ["aa"],
        "aa"     => ["b"],
      },
      "aacaa",
      [[1, "aca", "aa"],
       [0, "aaa", "aa"],
       [0, "aaa", "aa"],
       [0, "aa", "b"],
       [0, "b", ""],
      ]
    )
  end

  def test_solver_wildcard3()
    @max_moves = 10
    assert_solution(
      {
        "..."      => [""],
      },
      "abcdef",
      [
        [0, "abc", ""],
        [0, "def", ""]
      ]
    )
  end

  def test_solver_wildcard4()
    @max_moves = 10
    assert_solution(
      {
        ".."      => ["21"],
        "ba"      => [""],
      },
      "ab",
      [
        [0, "ab", "ba"],
        [0, "ba", ""]
      ]
    )
  end


end

