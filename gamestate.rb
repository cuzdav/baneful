
GS_PLAY_IDX = 0
GS_PLAY_PAT = 1
GS_PLAY_REPL = 2
GS_PLAY_CAPTURES = 3

SPECIAL_PATTERN_CHARS = ".123456789"
SPECIAL_REPL_CHARS = "123456789"

# similar to String.index but treats '.' as wildcard that matches anything
# note: each '.' is its own capture group, and return is a string of length N
# where N is the number of '.' in the pattern.  Each char is the captured value
# for the ith wildcard.
# RETURNS: a pair of values, index of match (or nil) and replacement chars as a string
def wc_index(str, pat, start_idx)
  while start_idx + pat.size <= str.size
    ridx = 0
    matches = true
    repls = ""
    pat.each_char do |ch|
      str_char = str[start_idx + ridx]
      if ch == '.'
        repls += str_char
      elsif str_char != ch
        matches = false
        break
      end
      ridx += 1
    end
    return start_idx, repls if matches
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


# given a str, the pat may refer to captures represented in repl_chars
# for any single digit D in (1...9), if str[i] == repl_chars[D] then it's a match
# presumably, repl_chars was returned from wc_index.
def wc_equals(str, pat, repl_chars)
  return false if pat.size != str.size
  idx = 0
  fixed_pat = wc_replace(pat, repl_chars)
  fixed_pat.each_char do |ch|
    return false if str[idx] != ch and ch != '.'
    idx += 1
  end
  return true
end


# independent of any UI, this is the actual core representation of the game and
# generates all possible moves according to the rules
class GameState
  attr_accessor :rules
  attr_accessor :cur_row
  attr_accessor :max_depth
  attr_reader :goal
  attr_reader :max_width
  attr_reader :prev_rows
  attr_reader :played_moves
  attr_accessor :verbose

  # rules = {<from> -> [<to>, ...]}
  # initial_str = staring position
  # num_moves (max turns)
  # max_width = limit to how wide row can grow when applying constructive rules
  # goal = win condition

  def initialize(rules, initial_str, num_moves, max_width = 7, goal = "")
    @verbose = false
    @rules = rules
    @goal = goal
    @cur_row = initial_str.dup
    @initial_row = initial_str
    @max_width = max_width
    @curdepth = 0
    @max_depth = 99
    @num_moves = num_moves
    @prev_rows = []
    @played_moves = []
  end

  def clone_from_cur_position()
    return GameState.new(@rules, @cur_row, @num_moves, @max_width, @goal)
  end

  def moves_remaining
    return @num_moves - @played_moves.size
  end

  def reset
    initialize(@rules, @initial_row, @num_moves, @max_width, @goal)
  end

  def make_move(offset, from, to)
    if moves_remaining == 0
      puts ("No moves remain") if @verbose
      return nil
    end
    if @max_depth == @prev_rows.size
      puts ("max_depth of #{@max_depth} reached") if @verbose
      return nil
    end
    if @cur_row.index(from, offset) != offset
      raise "invalid replacement: cur_row=#{@cur_row}, from=#{from}, offset=#{offset}"
    end
    if @cur_row.size - from.size + to.size > max_width
      raise "too wide: maxwidth= #{@max_width}"
    end

    @prev_rows.push(@cur_row.dup)
    @played_moves.push([offset, from, to])
    @cur_row[offset...offset+from.size] = to
    return @cur_row
  end

  def undo_move()
    return nil if @prev_rows.empty?
    @played_moves.pop
    @cur_row = @prev_rows.pop
  end

  def solved?()
    @cur_row == @goal
  end

  # return an array of possible moves.  A move is represented as:
  # [offset, from-str, to-str]
  # Special characters in rule strings affect options:
  # PATTERN:
  #   .  matches any single char
  # REPLACEMENT:
  #   1 2 3, ..., 9  : placeholder for whatever char matched Nth '.' in PATTERN
  def possible_plays()
    results = []

    # pat: string, repls: array of replacements
    rules.each do |pat, repls|
      base_len = @cur_row.size - pat.size
      repls.each do |replacement|
        if base_len + replacement.size <= max_width
          idx = 0
          loop do
            idx, repl_chars = wc_index(@cur_row, pat, idx)
            break if idx.nil?
            cur_pat, cur_repl = fixup_wildcards(pat, replacement ,repl_chars)
            # GS_PLAY_IDX,
            # GS_PLAY_PAT,
            # GS_PLAY_REPL
            # GS_PLAY_CAPTURES
            results << [idx, cur_pat, cur_repl, repl_chars]
            idx += 1
          end
        end
      end
    end
    return results
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
    (?1..).each do |placeholder|
      cur_repl.gsub!(placeholder, repl_chars[ridx])
      ridx += 1
      break if ridx == repl_chars.size
    end

    return cur_pat, cur_repl
  end

end

