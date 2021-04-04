require "test/unit"
require_relative "game_data.rb"


class GameStatTest < Test::Unit::TestCase

  def setup()
    @state = GameState.new(
      {
        "ab"  => ["bb"],  # =a  *b
        "ccc" => [""],    # -c
        "bc"  => ["c"],
        "d"   => ["dd"]   # +d
      },
      "",  # initial_str
      nil, # type_overrides
      10   # max_width of board
    )
  end

  def test_constructor
    rules = {
      "a" => ["abc"],
      "abc" => [""],
    }

    gs = GameState.new(
      rules,
      "a",
      nil, # type_overrides
      max_width = 9
    )
    assert_equal(rules, gs.rules)
    assert_equal("", gs.goal)
    assert_equal("a", gs.cur_row)
    assert_equal(max_width, gs.max_width)

    gs2 = gs.clone_from_cur_position
    gs = nil

    assert_equal(rules, gs2.rules)
    assert_equal("", gs2.goal)
    assert_equal("a", gs2.cur_row)
    assert_equal(max_width, gs2.max_width)
  end

  def test_reset
    rules = {
      "a" => ["abc"],
      "abc" => [""],
    }

    gs = GameState.new(
      rules,
      "a",
      nil, # type_overrides
      max_width = 9
    )
    gs.cur_row = "aaaa" # <<<<<<<<< modified
    assert_equal(rules, gs.rules)
    assert_equal("", gs.goal)
    assert_equal("aaaa", gs.cur_row)
    assert_equal(max_width, gs.max_width)

    gs.reset
    assert_equal(rules, gs.rules)
    assert_equal("", gs.goal)
    assert_equal("a", gs.cur_row) # <<<<< orig
    assert_equal(max_width, gs.max_width)
  end

  def test_possible_plays1()
    @state.cur_row = "abcccbccc"
    plays = @state.possible_plays()
#    assert_possible_plays_equal(
#      [
#        [0, "ab", "bb"],
#        [2, "ccc", ""],
#        [6, "ccc", ""],
#        [1, "bc", "c"],
#        [5, "bc", "c"],
#      ], plays)
#
    assert_possible_plays_equal(
      [
        move(idx:0, from:"ab", repl:"bb"),
        move(idx:2, from:"ccc", repl:""),
        move(idx:6, from:"ccc", repl:""),
        move(idx:1, from:"bc", repl:"c"),
        move(idx:5, from:"bc", repl:"c"),
      ], plays)


    # ok to grow
    @state.cur_row = "ffffffffd"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        move(idx:8, from:"d", repl:"dd")
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
      nil, # type_overrides
      10  # max_width of board
    )

    @state.cur_row = "abacac"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        move(idx:0, from:"aba", repl:"b", captures:"b", rawrepl: "1"),
        move(idx:2, from:"aca", repl:"c", captures:"c", rawrepl: "1"),
      ],
      plays
    )

    @state.cur_row = "cbabc"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        move(idx:1, from:"bab", repl:"aa", captures:"a", rawrepl: "11"),
      ],
      plays
    )
  end

  def test_possible_plays_wildcard()
    @state = GameState.new(
      {
        "a.a"  => ["1"],
        "a"    => ["c"],
        "b.b"  => ["11"],
      },
      "", # initial_str
      nil, # type_overrides
      10  # max_width of board
    )

    @state.cur_row = "abacac"
    plays = @state.possible_plays()
    assert_possible_plays_equal(
      [
        move(idx:0, from:"a",   repl:"c"),
        move(idx:2, from:"a",   repl:"c"),
        move(idx:4, from:"a",   repl:"c"),
        move(idx:0, from:"aba", repl:"b", captures:"b", rawrepl:"1"),
        move(idx:2, from:"aca", repl:"c", captures:"c", rawrepl:"1"),
      ],
      plays
    )

    plays = @state.possible_plays(true)
    assert_possible_plays_equal(
      [
        move(idx:0, from:"aba", repl:"b", captures:"b", rawrepl:"1"),
        move(idx:2, from:"aca", repl:"c", captures:"c", rawrepl:"1"),
      ],
      plays
    )

  end

  # converts a simple idx/from/to invocation into the slightly more cumbersome
  # (but more useful) array
  def make_move(idx, pat, repl, captures="")
    mv = move(idx:idx, from:pat, repl:repl, captures:captures)
    return @state.make_move(mv)
  end

  def move(idx:, from:, repl:, rawrepl: nil, captures: "")
    rawrepl = repl if rawrepl == nil
    return [idx, from, repl, rawrepl, captures]
  end

  def assert_possible_plays_equal(expected_plays, possible_plays_actual)

    # GS_PLAY_ARRAY
    expected = expected_plays.sort.map do |idx, pat, repl, rawrepl, capture|
      # GS_PLAY_ARRAY
      [idx, pat, repl, rawrepl, capture ? capture : ""]
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
  end

  def test_make_moves1()
    @state.cur_row = "abccc"
    assert_equal(0, @state.cur_move_number)
    assert_equal("abccc", @state.cur_row)
    assert_equal([], @state.prev_rows)

    move = make_move(0, "ab", "bb")
    assert_equal("bbccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(1, @state.cur_move_number)
    assert_equal(["abccc"], @state.prev_rows)

    move = make_move(1, "bc", "c")
    assert_equal("bccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(2, @state.cur_move_number)
    assert_equal(["abccc", "bbccc"], @state.prev_rows)

    move = make_move(0, "bc", "c")
    assert_equal("ccc", move)
    assert_equal(move, @state.cur_row)
    assert_equal(3, @state.cur_move_number)
    assert_equal(["abccc", "bbccc", "bccc"], @state.prev_rows)

    move = make_move(0, "ccc", "")
    assert_equal("", move)
    assert_equal(move, @state.cur_row)
    assert_equal(4, @state.cur_move_number)
    assert_equal(["abccc", "bbccc", "bccc", "ccc"], @state.prev_rows)
  end

  def test_make_moves_fail()
    @state.cur_row = "abccc"
    assert_equal("abccc", @state.cur_row)
    assert_equal(0, @state.cur_move_number)

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
    assert_equal(0, @state.cur_move_number)
    assert_equal("abccc", @state.cur_row)

    move = make_move(2, "ccc", "")
    assert_equal(move, @state.cur_row)
    assert_equal("ab", move)
    assert_equal(1, @state.cur_move_number)
    assert_equal(["abccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("abccc", @state.cur_row)
    assert_equal(0, @state.cur_move_number)
    assert_equal([], @state.prev_rows)

    move = make_move(0, "ab", "bb")
    move = make_move(1, "bc", "c")
    move = make_move(0, "bc", "c")
    move = make_move(0, "ccc", "")

    assert_equal("", @state.cur_row)
    assert_equal(4, @state.cur_move_number)
    assert_equal(["abccc", "bbccc", "bccc", "ccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("ccc", @state.cur_row)
    assert_equal(3, @state.cur_move_number)
    assert_equal(["abccc", "bbccc", "bccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("bccc", @state.cur_row)
    assert_equal(2, @state.cur_move_number)
    assert_equal(["abccc", "bbccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("bbccc", @state.cur_row)
    assert_equal(1, @state.cur_move_number)
    assert_equal(["abccc"], @state.prev_rows)

    @state.undo_move
    assert_equal("abccc", @state.cur_row)

    assert_equal(0, @state.cur_move_number)
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

  def test_rotating_cells

    type_overrides = {
      "x" => {
        "type" => "RotatingColors",
        "cycle_chars" => "abc"
      }
    }

    @state = GameState.new(
      {
        "a" => [""],
      },
      "axaa", # initial_str
      type_overrides,
      9  # max_width of board
    )

    # initial, x -> a
    assert_equal("aaaa", @state.cur_row)
    #              ^
    #              x

    # 1st 'a' is removed, x turns into b
    make_move(0, "a", "")
    assert_equal("baa", @state.cur_row)

    # last 'a' is removed, x turns into c
    make_move(2, "a", "")
    assert_equal("ca", @state.cur_row)

    # remove last 'a', and x wraps around and turns into 'a'
    make_move(1, "a", "")
    assert_equal("a", @state.cur_row)

    # solved
    make_move(0, "a", "")
    assert_equal("", @state.cur_row)

    # --------- UNDO
    @state.undo_move
    assert_equal("a", @state.cur_row)
    @state.undo_move
    assert_equal("ca", @state.cur_row)
    @state.undo_move
    assert_equal("baa", @state.cur_row)
    @state.undo_move
    assert_equal("aaaa", @state.cur_row)
  end

end
