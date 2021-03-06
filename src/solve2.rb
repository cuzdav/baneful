require_relative 'game_data'

# Has two separate algorithms that work together. 1) a top-down
# brute force search, starting at the game position this is the
# dynamic search 2) a pre-computed set of position starting at the
# solved state, then building backwards, applying every possible
# move to make all the 1-move-to-solve positions, then on top of
# those applying all possible 2-moves-to-solve positions, and so on,
# building "up" to a given depth. Then the solution is a simple
# lookup, without getting stuck in any dark corners of uselessness.
# Every path it finds is optimal Problem is here, WILDCARDS are not
# "blown up" in to every possible

# So, If the static lookup succeeds, it's super fast, but if it
# doesn't (due to use of wildcards in the solution) a dynamic search
# will work from the problem down to anything in the precomputed
# answers.  This means when they hook up, we have a complete answer.

class Solver
  attr_reader :positions
  attr_reader :max_width
  attr_reader :max_moves
  attr_reader :rev_rules

  def initialize(rules, max_moves, max_width)
    @rev_rules = create_reverse_mapping(rules)
    @positions = {}
    @max_width = max_width
    @max_moves = max_moves
    t1 = Time.now
    populate
    t2 = Time.now
    puts("Preprocessing took: #{t2 - t1}")
  end

  def find_solution(game_data)
    @best_solution = []
    @game_data = game_data.clone_from_cur_position
    move_data = @positions[game_data.cur_row]
    if move_data
      result = build_solution_path(move_data)
    else
      @visited = {}
      @move_number = 0
      find_solution_dynamic
      result = @best_solution || []
    end
    result
  end

  private

  #
  # This is a dynamic brute-force search FROM the player's current position
  # toward the final goal.  However, finding the empty string is not necessary,
  # since there are pre-built solutions (static solutions) for up to N moves
  # from the solution (not including wildcards), so all we really need to do is
  # brute force into _any_ of the solutions in the static solution set, because
  # the rest is known.  This speeds up the search _massively_.  But even after
  # finding a solution, keep searching for a little while because a shorter path
  # may still be found.
  #
  def find_solution_dynamic
    if @game_data.solved?
      update_best_maybe(nil)
      return
    end

    prev_move_num = @visited[@game_data.cur_row]
    if !prev_move_num.nil? && (prev_move_num <= @move_number)
      return
    end

    @visited[@game_data.cur_row] = @move_number

    moves = @game_data.possible_plays
    moves.each do |move|
      next unless @game_data.make_move(move)

      raw_pat, raw_repl = @game_data.get_raw_rule_and_repl_for_move(move)
      cur_pos = @game_data.cur_row.dup
      if raw_pat.include?('.')
        # replace the replaced chars with the raw (wildcard) pattern
        # this makes it able to hook into the set of pre-computed positions.
        cur_pos[move[GS_PLAY_IDX]...raw_repl.size] = raw_pat
      end

      # did our two solver techniques connect? (Brute force ran into a
      # pre-populated static answer if 'pos' exists)
      pos = @positions[cur_pos]
      unless pos.nil?
        # Success! Limited brute force search with wildcards was able to
        # find a solution in the precomputed set of positions!
        # DON'T BREAK because the first found solution may not be the best.
        update_best_maybe(pos)
      end

      @move_number += 1
      find_solution_dynamic
      @move_number -= 1
      @game_data.undo_move
    end
    nil
  end

  def build_solution_path(move_data)
    # walk the linked list of move-tuples, and put into an actual
    # array (The static solution's next field is in GS_PLAY_RESULT
    # index, so we can jump from position to position, toward the
    # solution by looking them up.)
    result = []
    n = 0
    while !move_data.nil? && (n < @max_moves)
      n += 1
      result << move_data # [idx, from, to, captures, next(result), num_moves]
      move_data = @positions[move_data[GS_PLAY_RESULT]]
    end
    result
  end

  def update_best_maybe(move_data)
    moves = @game_data.played_moves
    total_moves = moves.size + (move_data ? move_data[GS_PLAY_NUM_MOVES] : 0)
    return if !@best_solution.empty? && (total_moves >= @best_solution.size)

    static_solution = build_solution_path(move_data)
    unless static_solution.empty?
      # dynamic search met an non-empty static solution. The last of
      # the dynamic path is the same first move as the static, which
      # is how they were linked up. So remove the first one in
      # static, since if it's a wildcard it's unexpanded, and the
      # dynamic one is resolved.
      static_solution = static_solution[1..-1]
    end
    @best_solution = moves.clone + static_solution
    @game_data.max_depth = total_moves
  end

  def populate
    # This builds the static set of pre-populated solutions.
    # use double buffering to dequeue from one buffer and enqueue onto
    # another, then swap when we run out. When both are empty, we are
    # done. This allows for tracking the depth, so we know when we
    # swap we are at a new "tier" in answer-space: the number of moves
    # required to solve it has increased.

    # general approach:
    # 1) start with an empty string
    # 2) find every move that could result in current string before a replacement
    # 3) for each move, add the cells to the string, creating the position it would
    #    look like *before* that replacement was made.
    #    Example: if current string is "bb" and we have a rule "a" -> [""],
    #    then for that rule, there are 3 moves we could have made:
    #       abb -> bb
    #       bab -> bb
    #       bba -> bb
    #    each of these would be added to the set of solutions.

    # In short, we start empty, then insert all of the positions that are solvable
    # in 1 move.  Then on top of those, we insert every previous position that could
    # result in those 1-move-to-win positions.  Those are 2-moves-to-win.  And so on,
    # building a set of every solution "up" from the winning state backwards, to some
    # depth.

    # Due to an explosion of potential, the static set of solutions DOES NOT INCLUDE
    # WILDCARDS.  Those are left for dynamic search, which is trying to find a path
    # into any position in the static set (but looks for the best it can find)
    # Every position in the static set has a known path to the true solution,
    # so finding a position anywhere in this pre-built static set is equivalent to
    # finding a solution (though some paths are shorter than others.)
    q = []
    next_q = ['']
    move_count = 0

    while (move_count <= @max_moves) && !next_q.empty?
      move_count += 1
      q = next_q
      next_q = []

      # Current queue of positions requiring the same number of moves
      until q.empty?
        cur_row = q.pop

        # to help prevent solutions that exceed max-width of the grid
        width_remain = @max_width - cur_row.size
        # over every rule...
        @rev_rules.each do |from, to_list|
          from_size = from.size
          next_idx = 0

          # for every location in the current row where this rule applies...
          while (idx = cur_row.index(from, next_idx))
            next_idx = idx + 1
            # apply every replacement...
            to_list.each do |to|
              # ensuring result fits
              next unless to.size - from_size <= width_remain

              newrow = cur_row[0...idx] + to + cur_row[idx + from_size..-1]

              # ... and we haven't seen this position before
              # (because if we have, then seeing it again MUST mean we
              # are on a worse/longer path than before)
              next if @positions.has_key?(newrow)

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

              # GS_PLAY_ARRAY
              @positions[newrow] = [
                idx, # GS_PLAY_IDX
                to, # GS_PLAY_PAT
                from, # GS_PLAY_REPL
                from, # GS_PLAY_RAWREPL (no wildcards in static solutions)
                '', # GS_PLAY_CAPTURES
                cur_row, # GS_PLAY_RESULT
                move_count, # GS_PLAY_NUM_MOVES
              ]
            end
          end
        end
      end
    end
  end
end

# mainly for 1-off testing
def main()
  rules = { # a/bc/bcd/bcdd/bcda/bcdbc/bcbc/cbc/aa/
    'abcd' => [''],
    'a' => ['bc'],
    'bc' => ['bcd', 'c'],
    'd' => ['a', 'db'],
    'db' => ['b'],
    'cbc' => ['ab'],
    '...' => ['a']
  }
  rows= [
    'bd',
  ]
  moves = 10
  width = 7
  solver = Solver.new(rules, moves, width)
  game_data = GameState.new(rules, rows[0], width)
  solution = solver.find_solution(game_data)

  if !solution.nil?
    solution.each do |move|
      puts(move.to_s)
    end
  else
    puts 'No solution found'
  end
end


if __FILE__ == $0
  main
end

