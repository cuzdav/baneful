# when wildcards involved, each '.' in :pat is replaced, with i'th char
# in `capture` string.  captures.size = (count of '.' in pat)

# GS_PLAY_ARRAY  - locations in code referring to this array (grep-able)
GS_PLAY_IDX = 0   # index into row where to apply the move
GS_PLAY_PAT = 1   # wildcards resolved
GS_PLAY_REPL = 2  # placeholders resolved
GS_PLAY_RAWREPL = 3 # placeholders unresolved (wildcards intact)
GS_PLAY_CAPTURES = 4
GS_PLAY_RESULT = 5 # result of applying move to current row (opt)
GS_PLAY_NUM_MOVES = 6 # number of moves from the solution (opt)


SPECIAL_PATTERN_CHARS = "?123456789"
SPECIAL_REPL_CHARS = "123456789"

def create_reverse_mapping(rules)
  rev = {}
  rules.each do |from, to_list|
    to_list.each do |to|
      rev[to] ||= []
      rev[to] << from
    end
  end
  rev.values.each do |values|
    values.sort!
    values.uniq!
  end
  return rev
end

# similar to String.index but treats '.' as wildcard that matches anything
# note: each '.' is its own capture group, and return is a string of length N
# where N is the number of '.' in the pattern.  Each char is the captured value
# for the ith wildcard.
# RETURNS: a pair of values, index of match (or nil) and replacement chars as a string
def wc_index(str, pat, start_idx)
  size = str.size - pat.size
  while start_idx <= size
    ridx = start_idx
    repls = ""
    pat.each_char do |ch|
      str_char = str[ridx]
      if str_char != ch
        if ch != '.'
          ridx = nil
          break
        else
          repls += str_char
        end
      end
      ridx += 1
    end
    return start_idx, repls if ridx != nil
    start_idx += 1
  end
  return nil, ""
end

def wc_replace(str, repl_chars)
  idx = 0
  result = ""
  str.each_char do |ch|
    if (ch.ord >= ?1.ord and ch.ord <= ?9.ord)
      ridx = ch.ord - ?1.ord
      rep_ch = repl_chars[ridx]
      raise "invalid substitution #{ridx}" if rep_ch == nil
      result += rep_ch
    else
      result += ch
    end
    idx += 1
  end
  return result
end


# Compare two strings.  Pat _may_ contain . as a wildcard char, which matches
# any corresponding character in str at the same offset.
def wc_equals(str, pat)
  return false if pat.size != str.size
  idx = 0
  pat.each_char do |ch|
    return false if str[idx] != ch and ch != '.'
    idx += 1
  end
  return true
end

# given a str, the pat may refer to captures represented in repl_chars
# for any single digit D in (1...9), if str[i] == repl_chars[D] then it's a match
# presumably, repl_chars was returned from wc_index.
def wc_with_placeholder_equals(str, pat, repl_chars)
  return false if pat.size != str.size
  fixed_pat = wc_replace(pat, repl_chars)
  return wc_equals(str, fixed_pat)
end


# independent of any UI, this is the actual core representation of the game and
# generates all possible moves according to the rules
class GameState
  attr_accessor :rules
  attr_accessor :max_depth
  attr_reader :cur_row
  attr_reader :cur_raw_row
  attr_reader :goal
  attr_reader :max_width
  attr_reader :prev_rows
  attr_reader :played_moves
  attr_accessor :verbose

  def to_s
    "GameState:\n\trules:#{rules}\n\tcur_row: #{cur_row}"
  end

  # rules = {<from> -> [<to>, ...]}
  # initial_row_str = staring position
  # max_width = limit to how wide row can grow when applying constructive rules
  # goal = win condition

  def initialize(rules, initial_row_str, type_overrides=nil, max_width = 9, goal = "")
    @verbose = false
    @type_overrides = type_overrides || {}
    @rules = rules
    @goal = goal
    @cur_raw_row = initial_row_str.dup
    @initial_row = initial_row_str
    @max_width = max_width
    @max_depth = 99
    @prev_rows = []
    @played_moves = []

    if @type_overrides.class != Hash
      raise("Logic error, passed #{@type_overrides.class} for type_overrides, expecting hash")
    end

    @cur_row = compute_cur_row(@cur_raw_row)
    raise "Invalid max_width" if max_width == nil
  end

  def clone_from_cur_position()
    return GameState.new(
             @rules,
             @cur_row,
             @type_overrides,
             @max_width,
             @goal)
  end

  def cur_move_number()
    return @played_moves.size
  end

  def cur_raw_row=(cur_raw_row)
    @cur_raw_row = cur_raw_row
    @cur_row = compute_cur_row(cur_raw_row)
  end

  # normally the cur_row is computed from the raw row, but it's
  # "ok" if the cur_row doesn't transform when we pretend it's the
  # raw row.  (raw rows contains placeholder chars that may have special
  # rules applied to update them, while cur_row is resolved, and loses
  # that metadata.)
  def cur_row=(cur_row)
    if compute_cur_row(cur_row) != cur_row
      raise("overrides detected; must use cur_raw_row= instead")
    end
    @cur_raw_row = cur_row
    @cur_row = cur_row
  end

  def reset
    initialize(@rules, @initial_row, @type_overrides, @max_width, @goal)
  end

  # move_hash: same fields returned from each elt of possible_plays
  # all the gs_play_* fields from top of file
  def make_move(move)
    if @max_depth == @prev_rows.size
      puts ("max_depth of #{@max_depth} reached") if @verbose
      return nil
    end

    offset = move[GS_PLAY_IDX]
    from = move[GS_PLAY_PAT]
    to = move[GS_PLAY_REPL]

    if @cur_row.index(from, offset) != offset
      dump_plays
      raise "invalid replacement: cur_row=#{@cur_row}, from=#{from}, offset=#{offset}"
    end
    if @cur_row.size - from.size + to.size > max_width
      raise "too wide: maxwidth= #{@max_width}"
    end

    cached_move = move.dup

    cached_raw_row = @cur_raw_row.dup
    @prev_rows.push(cached_raw_row)
    @played_moves.push(cached_move)
    @cur_raw_row[offset...offset+from.size] = to
    @cur_row = compute_cur_row(@cur_raw_row)
    cached_row = @cur_row.dup
    cached_move[GS_PLAY_RESULT] = cached_row
    return @cur_row
  end

  def undo_move()
    return nil if @prev_rows.empty?
    @played_moves.pop
    @cur_raw_row = @prev_rows.pop
    @cur_row = compute_cur_row(@cur_raw_row)
  end

  def solved?()
    return @cur_row == @goal
  end

  # return an array of possible moves.  a move is represented as:
  # [offset, from-str, to-str]
  # special characters in rule strings affect options:
  # pattern:
  #   ?  matches any single char
  # replacement:
  #   1 2 3, ..., 9  : placeholder for whatever char matched corresponding N in pattern
  def possible_plays()
    results = []
    # pat: string, repls: array of replacements
    rulenum = -1
    rules.each do |pat, repls|
      rulenum += 1
      base_len = @cur_row.size - pat.size
      repls.each do |replacement|
        replacement = "" if replacement == nil
        if base_len + replacement.size <= @max_width
          idx = 0
          loop do
            idx, repl_chars = wc_index(@cur_row, pat, idx)
            break if idx.nil?
            cur_pat, cur_repl = fixup_wildcards(pat, replacement, repl_chars)

            # Grepable key: GS_PLAY_ARRAY
            results << [idx, cur_pat, cur_repl, replacement, repl_chars]
            idx += 1
          end
        end
      end
    end
    return results
  end

  # if the rule has an :_overrides key, then it should be of the form:
  # {
  #    "type": "<name",
  #    ...
  # }
  # Where fields depend on the type.
  # Currently supported types are:
  #
  # "rotating", meaning to cycle through n pieces (chars), one per turn.
  #    fields: "cycle_chars": "abc" (string)
  #    Requires: cycle_chars.size == 2 or 3
  def compute_cur_row(cur_raw_row)
    return cur_raw_row if @type_overrides == nil

    charmap = make_charmap()
    cur_row = ""
    cur_raw_row.each_char do |char|
      translated  = charmap[char]
      cur_row += translated ? translated : char
    end
    return cur_row
  end

  # for the current turn in the current state, each
  # special cell char must map to ... SOMETHING
  # so build a map to pre-resolve those transformations
  # (They may be different each turn.)
  def make_charmap()
    charmap = {}
    @type_overrides.each do |char, char_override|
      type = char_override["type"]
      case type
      # cycle through the string of chars in cycle_chars
      when "RotatingColors"
        cycle_chars = char_override["cycle_chars"]
        if cycle_chars == nil
          raise "Invalid/missing 'cycle_chars' attribute of 'RotatingColors' type_override"
        end
        idx = cur_move_number % cycle_chars.size
        charmap[char] = cycle_chars[idx]
      else
        raise "Unhandled override type '#{type}'"
      end
    end
    return charmap
  end

  # find a rule that can produce the given move, matching the from (pat)
  # and containing the replace pattern.  same format as moves returned from
  # possible_plays in this class.
  #
  # note:the move provided is fully resolved, so matching it to a rule requires
  # accounting for wildcards in the rule code. (That is, it's not directly
  # obvious that a move came from a wildcard, but it needs to be figured out.)
  def get_raw_rule_and_repl_for_move(move)
    resolved_from_str = move[GS_PLAY_PAT]
    resolved_to_str = move[GS_PLAY_REPL]

    @rules.each do |raw_from_str, raw_repl_strs|
      idx, captures = wc_index(resolved_from_str, raw_from_str, 0)
      if idx != nil and raw_from_str.size == resolved_from_str.size
        # rule pattern matches, but does a repl also match? given wildcards,
        # it's possible this rule matches the from_str pat, but has no
        # replacement that matches. (Must be from a different rule, if it can
        # match multiple rules.)
        raw_repl_strs.each do |raw_repl|
          if wc_with_placeholder_equals(resolved_to_str, raw_repl, captures)
            return raw_from_str, raw_repl
          end
        end
      end
    end
    return nil, nil
  end

  def fixup_wildcards(pat, repl, repl_chars)
    if repl_chars.size == 0
      return pat, repl
    end

    pidx = 0
    cur_pat = pat.dup.gsub(".") do |_|
      pidx += 1
      repl_chars[pidx-1]
    end

    cur_repl = repl.dup
    ridx = 0
    placeholder = ?1
    loop do
      ch = repl_chars[ridx]
      break if ch == nil
      cur_repl.gsub!(placeholder, repl_chars[ridx])
      ridx += 1
      placeholder = placeholder.next
      break if ridx == repl_chars.size
    end

    return cur_pat, cur_repl
  end

  def dump_plays
    puts("[Moves Played - initial state: #{@initial_row}]")

    move_num = 0

    #GS_PLAY_ARRAY
    @played_moves.each do |idx, pat, repl, rawrepl, captures, result, _|
      repl_str = repl.empty? ? "EMPTY" : repl
      raw_str = rawrepl != repl ? "(raw:#{rawrepl})" : ""
      captures = (captures.nil? or captures.empty?) ? "" : ", captures:#{captures})"
      puts("[#{move_num}]: [idx:#{idx} from:#{pat} " +
           "repl:#{repl_str} #{raw_str}#{captures} " +
           "RESULT:#{result}")
      move_num += 1
    end
  end

end

