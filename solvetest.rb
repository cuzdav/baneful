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


  def test_possible_plays1()
    @state.cur_row = "=*---*---"
    plays = @state.possible_plays()
    assert_equal(
      [
        [0, "=*", "**"],
        [2, "---", ""],
        [6, "---", ""],
        [1, "*-", "-"],
        [5, "*-", "-"]
      ].sort, plays.sort)

    # ok to grow
    @state.cur_row = "01234567+"
    plays = @state.possible_plays()
    assert_equal(
      [
        [8, "+", "++"]
      ].sort, plays.sort)

    # grows too wide (max_width = 10)
    @state.cur_row = "012345678+"
    plays = @state.possible_plays()
    assert_equal([], plays)
  end

  def test_clone()
    c = @state.clone()
    assert_equal(c.rules, @state.rules)
    assert_equal(c.cur_row, @state.cur_row)
    assert_equal(c.goal, @state.goal)
    assert_equal(c.max_width, @state.max_width)
    assert_equal(c.max_depth, @state.max_depth)
    assert_equal(c.moves_remaining, @state.moves_remaining)
  end

  def test_make_moves1()
    @state.cur_row = "=*---"
    assert_equal(10, @state.moves_remaining)
    assert_equal("=*---", @state.cur_row)
    assert_equal([], @state.prev_rows)

    move = @state.make_move(0, "=*", "**")
    assert_equal("**---", move)
    assert_equal(move, @state.cur_row)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["=*---"], @state.prev_rows)

    move = @state.make_move(1, "*-", "-")
    assert_equal("*---", move)
    assert_equal(move, @state.cur_row)
    assert_equal(8, @state.moves_remaining)
    assert_equal(["=*---", "**---"], @state.prev_rows)

    move = @state.make_move(0, "*-", "-")
    assert_equal("---", move)
    assert_equal(move, @state.cur_row)
    assert_equal(7, @state.moves_remaining)
    assert_equal(["=*---", "**---", "*---"], @state.prev_rows)

    move = @state.make_move(0, "---", "")
    assert_equal("", move)
    assert_equal(move, @state.cur_row)
    assert_equal(6, @state.moves_remaining)
    assert_equal(["=*---", "**---", "*---", "---"], @state.prev_rows)
  end

  def test_make_moves_fail()
    @state.cur_row = "=*---"
    assert_equal("=*---", @state.cur_row)
    assert_equal(10, @state.moves_remaining)

    begin
      move = @state.make_move(0, "---", "")
      fail
    rescue
    end

    begin
      move = @state.make_move(1, "=*", "")
      fail
    rescue
    end
  end

  def test_undo()
    @state.cur_row = "=*---"
    assert_equal(10, @state.moves_remaining)
    assert_equal("=*---", @state.cur_row)

    move = @state.make_move(2, "---", "")
    assert_equal(move, @state.cur_row)
    assert_equal("=*", move)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["=*---"], @state.prev_rows)

    @state.undo_move
    assert_equal("=*---", @state.cur_row)
    assert_equal(10, @state.moves_remaining)
    assert_equal([], @state.prev_rows)

    move = @state.make_move(0, "=*", "**")
    move = @state.make_move(1, "*-", "-")
    move = @state.make_move(0, "*-", "-")
    move = @state.make_move(0, "---", "")

    assert_equal("", @state.cur_row)
    assert_equal(6, @state.moves_remaining)
    assert_equal(["=*---", "**---", "*---", "---"], @state.prev_rows)

    @state.undo_move
    assert_equal("---", @state.cur_row)
    assert_equal(7, @state.moves_remaining)
    assert_equal(["=*---", "**---", "*---"], @state.prev_rows)

    @state.undo_move
    assert_equal("*---", @state.cur_row)
    assert_equal(8, @state.moves_remaining)
    assert_equal(["=*---", "**---"], @state.prev_rows)

    @state.undo_move
    assert_equal("**---", @state.cur_row)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["=*---"], @state.prev_rows)

    @state.undo_move
    assert_equal("=*---", @state.cur_row)
    assert_equal(10, @state.moves_remaining)
    assert_equal([], @state.prev_rows)
    assert_nil(@state.undo_move)
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

