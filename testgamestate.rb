require "test/unit"
require_relative "gamestate.rb"


class GameStatTest < Test::Unit::TestCase

  def setup()
    @state = GameState.new(
      {
        "ab"  => ["bb"],  # =a  *b
        "ccc" => [""],    # -c
        "bc"  => ["c"],
        "d"   => ["dd"]   # +d
      },
      "", # initial_str
      10, # num_moves
      10  # max_width of board
    )
  end


  def test_possible_plays1()
    @state.cur_row = "abcccbccc"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        [0, "ab", "bb"],
        [2, "ccc", ""],
        [6, "ccc", ""],
        [1, "bc", "c"],
        [5, "bc", "c"]
      ], plays)

    # ok to grow
    @state.cur_row = "ffffffffd"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        [8, "d", "dd"]
      ], plays)

    # grows too wide (max_width = 10)
    @state.cur_row = "fffffffffd"
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
    assert_possible_plays_equal(
      [
        [0, "aba", "b", "b"],
        [2, "aca", "c", "c"],
      ],
      plays
    )

    @state.cur_row = "cbabc"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        [1, "bab", "aa", "a"],
      ],
      plays
    )
  end

  def test_possible_plays_wildcard_only()
    @state = GameState.new(
      {
        "a.a"  => ["1"],
        "a"    => ["c"],
        "b.b"  => ["11"],
      },
      "", # initial_str
      10, # num_mo411ves
      10  # max_width of board
    )

    @state.cur_row = "abacac"
    plays = @state.possible_plays()
    puts("***#{plays}")
    assert_possible_plays_equal(
      [
        [0, "a",   "c", ""],
        [2, "a",   "c", ""],
        [4, "a",   "c", ""],
        [0, "aba", "b", "b"],
        [2, "aca", "c", "c"],
      ],
      plays
    )

    plays = @state.possible_plays(true)
    assert_possible_plays_equal(
      [
        [0, "aba", "b", "b"],
        [2, "aca", "c", "c"],
      ],
      plays
    )

  end

  # converts a simple idx/from/to invocation into the slightly more cumbersome
  # (but more useful) hash
  def make_move(idx, pat, repl, captures="")
    return @state.make_move([idx, pat, repl, captures])
  end

  def assert_possible_plays_equal(expected_plays, possible_plays_actual)
    expected = expected_plays.sort.map do |idx, pat, repl, capture|
      [idx, pat, repl, capture ? capture : ""]
    end
    actual = possible_plays_actual.sort
    assert_equal(expected, actual)
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
    @state.cur_row = "abccc"
    assert_equal(10, @state.moves_remaining)
    assert_equal("abccc", @state.cur_row)
    assert_equal([], @state.prev_rows)

    move = make_move(0, "ab", "bb")
    assert_equal("bbccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["abccc"], @state.prev_rows)

    move = make_move(1, "bc", "c")
    assert_equal("bccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(8, @state.moves_remaining)
    assert_equal(["abccc", "bbccc"], @state.prev_rows)

    move = make_move(0, "bc", "c")
    assert_equal("ccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(7, @state.moves_remaining)
    assert_equal(["abccc", "bbccc", "bccc"], @state.prev_rows)

    move = make_move(0, "ccc", "")
    assert_equal("", move)
    assert_equal(move, @state.cur_row)
    assert_equal(6, @state.moves_remaining)
    assert_equal(["abccc", "bbccc", "bccc", "ccc"], @state.prev_rows)
  end

  def test_make_moves_fail()
    @state.cur_row = "abccc"
    assert_equal("abccc", @state.cur_row)
    assert_equal(10, @state.moves_remaining)

    begin
      move = make_move(0, "ccc", "")
      fail
    rescue
    end

    begin
      move = make_move(1, "ab", "")
      fail
    rescue
    end
  end

  def test_undo()
    @state.cur_row = "abccc"
    assert_equal(10, @state.moves_remaining)
    assert_equal("abccc", @state.cur_row)

    move = make_move(2, "ccc", "")
    assert_equal(move, @state.cur_row)
    assert_equal("ab", move)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["abccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("abccc", @state.cur_row)
    assert_equal(10, @state.moves_remaining)
    assert_equal([], @state.prev_rows)

    move = make_move(0, "ab", "bb")
    move = make_move(1, "bc", "c")
    move = make_move(0, "bc", "c")
    move = make_move(0, "ccc", "")

    assert_equal("", @state.cur_row)
    assert_equal(6, @state.moves_remaining)
    assert_equal(["abccc", "bbccc", "bccc", "ccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("ccc", @state.cur_row)
    assert_equal(7, @state.moves_remaining)
    assert_equal(["abccc", "bbccc", "bccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("bccc", @state.cur_row)
    assert_equal(8, @state.moves_remaining)
    assert_equal(["abccc", "bbccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("bbccc", @state.cur_row)
    assert_equal(9, @state.moves_remaining)
    assert_equal(["abccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("abccc", @state.cur_row)
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

  def test_wc_with_placeholder_equals
    assert_equal(true, wc_with_placeholder_equals("aba", "a1a", "b"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a1a", "bc"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a2a", "mb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a3a", "mnb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a4a", "mnob"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a5a", "mnopb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a6a", "mnopqb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a7a", "mnopqrb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a8a", "mnopqrsb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "a9a", "mnopqrstb"))
    assert_equal(true, wc_with_placeholder_equals("aba", "121", "ab"))
    assert_equal(true, wc_with_placeholder_equals("aba", "212", "ba"))


    assert_equal(false, wc_with_placeholder_equals("aba", "a1a", "c"))
    assert_equal(false, wc_with_placeholder_equals("aba", "a2a", "bc"))
    assert_raise(RuntimeError) {
      wc_with_placeholder_equals("aba", "a9a", "x")
    }
  end

  def test_wc_equals
    # . has no special meaning on LHS, only on RHS
    assert_equal(true, wc_equals("aba", "aba"))
    assert_equal(true, wc_equals("aba", ".ba"))
    assert_equal(true, wc_equals("aba", "a.a"))
    assert_equal(true, wc_equals("aba", "ab."))
    assert_equal(true, wc_equals("aba", "..a"))
    assert_equal(true, wc_equals("aba", "a.."))
    assert_equal(true, wc_equals("aba", "..."))
    assert_equal(true, wc_equals("...", "..."))

    assert_equal(false, wc_equals("...", ""))
    assert_equal(false, wc_equals("a.a", "aba"))
    assert_equal(false, wc_equals("aba", "abaa"))
    assert_equal(false, wc_equals("aba", "ab"))
    assert_equal(false, wc_equals("aba", ""))
  end

end
