require_relative 'gamestate'

S_MOVE_IDX = 0
S_MOVE_PAT = 1
S_MOVE_REPL = 2
S_MOVE_CAPTURES = 3
S_MOVE_RESULT = 4 # position after applying repl for pat at idx
S_MOVE_NUM_MOVES = 5


class Solver

  # all the positions requiring 1 move to solve, then on top of those, all the
  # positions requiring 2 moves to solve, and so on, up to max_depth This is
  # *not* comprehensive, as wildcard moves are are present but still in an
  # encoded form.  They require dynamic searching to utilize.
  attr_reader :positions

  #TODO: reverse max_width and max_moves, to be consistent with gamestate,
  # OR just take a gamestate and remember it.  (I like this)
  def initialize(rules, rows, max_width, max_moves)
    @rules = create_reverse_mapping(rules)
    @positions = {}
    @max_width = max_width
    @max_moves = max_moves
    t1 = Time.now
    populate()
    t2 = Time.now
    puts("Preprocessing took: #{t2 - t1}")
  end

  def find_solution(game_state)
    @best_solution = []
    @game_state = game_state.clone_from_cur_position()
    move_data = @positions[game_state.cur_row]
    if move_data
      result = build_solution_path(move_data)
    else
      @visited = {}
      @move_number = 0
      find_solution_dynamic()
      result = @best_solution
    end
    return result
  end

  private

  # find_solution_dynamic is based on code from original solver, which worked
  # well but was too slow getting stuck checking out too many dark corners in
  # certain types of levels with a lot of loops and recursion. Its premise is
  # to start with the problem, and work toward the solution via brute force.

  # The pre-populated solver works fantastically for non-wildcard solutions, but
  # introducing wildcards causes a blowup of solutions it must pre-populate.
  # This approach is to start at the solution, and reverse all possible moves
  # from that position, yielding positions solvable with 1-move. Then for each
  # of those, reverse every next possible move yielding every 2-move solvable
  # positions etc. What is left is an optimal lookup table for solutions up to N
  # solutions deep. Wildcards makes this huge; otherwise is remains fairly small.

  # Now I'm trying to merge the two such that all the "static" solutions are
  # pre-computed as before, and used when possible. But if a solution isn't
  # found (due to a wildcard) it does a brute force from the "old" technique.
  # However, instead of finding THE solution via brute force, it only needs to
  # find any solution that was in the pre-computed pool. (Though the first it
  # finds might not be the best, it's at least a solution, and keeps searching.)
  def find_solution_dynamic()
    if @game_state.solved?
      update_best_maybe(nil)
      return
    end

    prev_move_num = @visited[@game_state.cur_row]
    if prev_move_num != nil and prev_move_num <= @move_number
      return
    end
    @visited[@game_state.cur_row] = @move_number

    moves = @game_state.possible_plays()
    moves.each do |move|
      if @game_state.make_move(move)
        raw_pat, raw_repl = @game_state.get_raw_rule_and_repl_for_move(move)
        if raw_pat.include?('.')
          cur_pos = @game_state.cur_row.dup
          # replace the replaced chars with the raw (wildcard) pattern
          # this makes it able to hook into the set of pre-computed positions.
          cur_pos[move[GS_PLAY_IDX]...raw_repl.size] = raw_pat
        else
          cur_pos = @game_state.cur_row.dup
        end

        # did our two solver techniques connect? (Brute force found a
        # prepopulated answer)
        pos = @positions[cur_pos]
        if pos != nil
          # Success! Limited brute force search with wildcards was able to
          # find a solution in the precomputed set of positions!
          update_best_maybe(pos)
        end

        @move_number += 1
        find_solution_dynamic()
        @move_number -= 1
        @game_state.undo_move()
      end
    end
    return
  end


  def build_solution_path(move_data)

    # walk the linked list of tuples, and put into an actual array
    result = []
    n = 0
    while move_data != nil and n < @max_moves
      n += 1
      result << move_data # [idx, from, to, captures, next(result), num_moves]
      move_data = @positions[move_data[S_MOVE_RESULT]]
    end
    return result
  end

  def update_best_maybe(move_data)
    moves = @game_state.played_moves
    total_moves = moves.size + (move_data ? move_data[S_MOVE_NUM_MOVES] : 0)
    if @best_solution.empty? or total_moves < @best_solution.size
      static_solution = build_solution_path(move_data)
      if not static_solution.empty?
        # dynamic search met an non-empty static solution.
        # The last of the dynamic path is the same first move as the
        # static, which is how they were linked up.  So remove the
        # one in static, since if it's a wildcard it's unexpanded,
        # and the dynamic one is resolved.
        static_solution = static_solution[1..]
      end
      @best_solution = moves.clone + static_solution
      @game_state.max_depth = total_moves
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
                  # Also, there are no captures because these are only moves
                  # that do not involve wildcards.
                  @positions[newrow] = [
                    idx,           # S_MOVE_IDX
                    to,            # S_MOVE_PAT
                    from,          # S_MOVE_REPL
                    "",            # S_CAPTURES
                    cur_row,       # S_MOVE_RESULT
                    move_count,    # S_MOVE_NUM_MOVES
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


def main()
  rules = { # a/bc/bcd/bcdd/bcda/bcdbc/bcbc/cbc/aa/
    "abcd" => [""],
    "a" => ["bc"],
    "bc" => ["bcd", "c"],
    "d" => ["a", "db"],
    "db" => ["b"],
    "cbc" => ["ab"],
    "..." => ["a"]
  }
  rows= [
    "bd",
  ]
  moves = 10
  width = 7
  solver = Solver.new(rules, rows, moves, width)
  game_state = GameState.new(rules, rows[0], 10, 15)
  solution = solver.find_solution(game_state)

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

