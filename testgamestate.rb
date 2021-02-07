require "test/unit"
require_relative "gamestate.rb"


class GameStatTest < Test::Unit::TestCase

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
        [0, "=*", "**", ""],
        [2, "---", "", ""],
        [6, "---", "", ""],
        [1, "*-", "-", ""],
        [5, "*-", "-", ""]
      ].sort, plays.sort)

    # ok to grow
    @state.cur_row = "01234567+"
    plays = @state.possible_plays()
    assert_equal(
      [
        [8, "+", "++", ""]
      ].sort, plays.sort)

    # grows too wide (max_width = 10)
    @state.cur_row = "012345678+"
    plays = @state.possible_plays()
    assert_equal([], plays)
  end

  def test_possible_plays2()
    @state = GameState.new(
      {
        "a.a"  => ["1"],
        "b.b"  => ["11"],
      },
      "", # initial_str
      10, # num_moves
      10  # max_width of board
    )

    @state.cur_row = "abacac"
    plays = @state.possible_plays()
    assert_equal(
      [
        [0, "aba", "b", "b"],
        [2, "aca", "c", "c"],
      ].sort,
      plays.sort)

    @state.cur_row = "cbabc"
    plays = @state.possible_plays()
    assert_equal(
      [
        [1, "bab", "aa", "a"],
      ].sort,
      plays.sort)

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

  def test_wc_index_nowildcards_matches_char()
    idx, repl_chars = wc_index("abcabc", "a", 0)
    assert_equal(0, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "b", 0)
    assert_equal(1, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "c", 0)
    assert_equal(2, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "a", 1)
    assert_equal(3, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "b", 2)
    assert_equal(4, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "c", 3)
    assert_equal(5, idx)
    assert_equal("", repl_chars)
  end

  def test_wc_index_nowildcards_doesnt_match_char()
    idx, repl_chars = wc_index("abcabc", "x", 0)
    assert_equal(nil, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "a", 4)
    assert_equal(nil, idx)
    assert_equal("", repl_chars)
  end

  def test_wc_index_nowildcards_matches_multichar()
    idx, repl_chars = wc_index("abcabc", "ab", 0)
    assert_equal(0, idx)
    assert_equal("", repl_chars)
    idx, repl_chars = wc_index("abcabc", "abc", 0)
    assert_equal(0, idx)
    assert_equal("", repl_chars)


    idx, repl_chars = wc_index("abcabc", "bc", 0)
    assert_equal(1, idx)
    assert_equal("", repl_chars)
    idx, repl_chars = wc_index("abcabc", "bca", 0)
    assert_equal(1, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "cab", 0)
    assert_equal(2, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "ab", 1)
    assert_equal(3, idx)
    assert_equal("", repl_chars)
    idx, repl_chars = wc_index("abcabc", "abc", 1)
    assert_equal(3, idx)
    assert_equal("", repl_chars)

    idx, repl_chars = wc_index("abcabc", "bc", 2)
    assert_equal(4, idx)
    assert_equal("", repl_chars)
  end

  def test_wc_index_with_wildcards_matches()

    idx, repl_chars = wc_index("a", ".", 0)
    assert_equal(0, idx)
    assert_equal("a", repl_chars)

    idx, repl_chars = wc_index("aba", ".", 0)
    assert_equal(0, idx)
    assert_equal("a", repl_chars)

    idx, repl_chars = wc_index("abc", "a.c", 0)
    assert_equal(0, idx)
    assert_equal("b", repl_chars)

    idx, repl_chars = wc_index("aaabc", "a.c", 0)
    assert_equal(2, idx)
    assert_equal("b", repl_chars)

    idx, repl_chars = wc_index("aaabcaxyc", "a..c", 0)
    assert_equal(1, idx)
    assert_equal("ab", repl_chars)

    idx, repl_chars = wc_index("aaabcaxyc", "a..c", 2)
    assert_equal(5, idx)
    assert_equal("xy", repl_chars)
  end

  def test_wc_equals
    assert_equal(true, wc_equals("aba", "a1a", "b"))
    assert_equal(true, wc_equals("aba", "a1a", "bc"))
    assert_equal(true, wc_equals("aba", "a2a", "mb"))
    assert_equal(true, wc_equals("aba", "a3a", "mnb"))
    assert_equal(true, wc_equals("aba", "a4a", "mnob"))
    assert_equal(true, wc_equals("aba", "a5a", "mnopb"))
    assert_equal(true, wc_equals("aba", "a6a", "mnopqb"))
    assert_equal(true, wc_equals("aba", "a7a", "mnopqrb"))
    assert_equal(true, wc_equals("aba", "a8a", "mnopqrsb"))
    assert_equal(true, wc_equals("aba", "a9a", "mnopqrstb"))
    assert_equal(false, wc_equals("aba", "121", "ab"))
    assert_equal(false, wc_equals("aba", "212", "ba"))


    assert_equal(false, wc_equals("aba", "a1a", "c"))
    assert_equal(false, wc_equals("aba", "a2a", "bc"))
    assert_equal(false, wc_equals("aba", "a9a", "x"))
  end

end
