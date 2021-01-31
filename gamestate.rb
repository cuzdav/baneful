
GS_PLAY_IDX = 0
GS_PLAY_PAT = 1
GS_PLAY_REPL = 2

# independent of any UI, this is the actual
# core representation of the game and enforcement of the rules

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
  # [offset, from-str, to-xsstr]
  def possible_plays()
    results = []

    # pat: string, subs: array of substitions
    rules.each do |pat, subs|
      base_len = @cur_row.size - pat.size
      subs.each do |replacement|
        if base_len + replacement.size <= max_width
          idx = 0
          loop do
            idx = @cur_row.index(pat, idx)
            break if idx.nil?
            results << [idx, pat, replacement]
            idx += 1
          end
        end
      end
    end
    return results
  end
end

