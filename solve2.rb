require_relative 'gamestate'

S_MOVE_IDX = 0
S_MOVE_PAT = 1
S_MOVE_REPL = 2
S_MOVE_RESULT = 3 # position after applying repl for pat at idx
S_MOVE_NUM_MOVES = 4
S_MOVE_RAW_PAT = 5
S_MOVE_RAW_REPL = 6
S_MOVE_CAPTURES = 7

class Solver

  # all the positions requiring 1 move to solve, then on top of those, all the
  # positions requiring 2 moves to solve, and so on, up to max_depth This is
  # *not* comprehensive, as wildcard moves are are present but still in an
  # encoded form.  They require dynamic searching to utilize.
  attr_reader :positions

  def initialize(rules, rows, max_width, max_moves)
    @rules = create_reverse_mapping(rules)
    @positions = {}
    @max_width = max_width
    @max_moves = max_moves
    populate()
    puts ("@positions size = #{@positions.size}")
  end

  def find_solution(game_state)
    move_data = @positions[row]
    if move_data == nil
      @visited = {}
      @game_state = game_state.clone_from_cur_position()
      move_data = find_solution_dynamic()
    end
    convert_solution_to_external_format(move_data)
    return result
  end

  private

  def find_solution_dynamic()
    if @game_state.solved?
      moves = @game_state.played_moves
      if @best_solution == nil or moves.size < @best_solution.size
        @best_solution = moves.clone
        @game_state.max_depth = @best_solution.size
      end
      return
    end

    prev_move_num = @visited[@game_state.cur_row]
    if prev_move_num != nil and prev_move_num <= @move_number
      return
    end
    @visited[@game_state.cur_row] = @move_number

    moves = @game_state.possible_plays
    moves.each do |move|
      if @game_state.make_move(move)
        @move_number += 1
        find_solution_impl()
        @move_number -= 1
        @game_state.undo_move()
      end
    end
  end


  def convert_solution_to_external_format(move_data)
    # internally the moves are stored in arrays, but outside they are hashes
    # this saves quite a bit of memory since only the actual path needs to
    # become a hash, instead of every prepopulated possible solution.
    result = []
    n = 0
    while move_data != nil and n < @max_moves
      n += 1
      move = {
        GS_PLAY_IDX      => move_data[S_MOVE_IDX],
        GS_PLAY_PAT      => move_data[S_MOVE_PAT],
        GS_PLAY_REPL     => move_data[S_MOVE_REPL],
        GS_PLAY_CAPTURES => move_data[S_MOVE_CAPTURES],
        :result          => move_data[S_MOVE_RESULT],
        :num_moves       => move_data[S_MOVE_NUM_MOVES],
      }
      result << move # [idx, from, to, next, moves]
      move_data = @positions[move_data[3]]
    end

  end

  def populate()
    # use double buffering to dequeue from one buffer and enqueue onto another,
    # then swap when we run out.  When both are empty, we are done.
    # This allows for tracking the depth, so we know when we swap we are at
    # a new "tier" in answer-space: the number of moves required to solve it
    # has increased.
    q = []
    next_q = [""]
    move_count = 0

    while move_count <= @max_moves and not next_q.empty?
      move_count += 1
      q = next_q
      next_q = []

      # Current queue of positions requiring the same number of moves
      while not q.empty?
        cur_row = q.pop

        # to help prevent solutions that exceed max-width of the grid
        width_remain = @max_width - cur_row.size
        # over every rule...
        @rules.each do |from, to_list|
          from_size = from.size
          next_idx = 0

          # for every location in the current row where this rule applies...
          while (idx = cur_row.index(from, next_idx)) != nil
            next_idx = idx + 1
            # apply every replacement...
            to_list.each do |to|
              # ensuring result fits
              if to.size - from_size <= width_remain
                newrow = cur_row[0...idx] + to + cur_row[idx + from_size..]

                #... and we haven't seen this position before
                # (because if we have, then seeing it again MUST mean we
                # are on a worse/longer path than before)
                if not @positions.has_key?(newrow)
                  # queue this up for processing as part of the work for the
                  # next group of solutions at N+1 (moves) compared to cur_row.
                  next_q.push(newrow)

                  # Note, to/from are reversed because we create them with a
                  # reverse map (to build up positions from an empty board), but
                  # we enqueue the moves that should be done to move closer to
                  # the solution. include "cur_row" so that it forms a linked
                  # list from any given position toward the solution (cur_row is
                  # one better than whatever position they are in before
                  # applying this move)
                  @positions[newrow] = [
                    idx,           # S_MOVE_IDX
                    to,            # S_MOVE_PAT
                    from,          # S_MOVE_REPL
                    cur_row,       # S_MOVE_RESULT
                    move_count,    # S_MOVE_NUM_MOVES
                    to,            # S_MOVE_RAW_PAT
                    from,          # S_MOVE_RAW_REPL
                    nil,           # S_MOVE_CAPTURES
                 ]
                end
              end
            end
          end
        end
      end
    end
  end
end


def main
  rules = {
    "a" => ["b"],
    "." => [""],
  }

  rows = ["abc"]

  solver = Solver.new(rules, rows, 7, 10)

  solution = solver.find_solution(rows[0])

  if solution != nil
    solution.each do |move|
      puts("#{move}")
    end
  else
    puts "No solution found"
  end
end


if __FILE__ == $0
  main
end
