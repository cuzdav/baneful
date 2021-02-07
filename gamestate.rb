
GS_PLAY_IDX = 0
GS_PLAY_PAT = 1
GS_PLAY_REPL = 2

SPECIAL_PATTERN_CHARS = "."
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

    # take all string keys and array of string values, make a big single string and
    # sort the chars, joining back to a string and remove duplicates
    @unique_chars = (rules.keys + rules.values.flatten).join.chars.sort.join.squeeze
    puts("rules: #{rules}, UNIQUE CHARS: #{@unique_chars}")
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
      raise "invalid replacement"
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

            # handle wildcards as necessary
            if repl_chars.size > 0
              replacement = replacement.dup
              ridx = 0
              (?1..).each do |placeholder| 
                replacement.gsub!(placeholder, repl_chars[ridx])
                ridx += 1
                break if ridx == repl_chars.size
              end
            end

            results << [idx, pat, replacement]
            idx += 1
          end
        end
      end
    end
    return results
  end

end

