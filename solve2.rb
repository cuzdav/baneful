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

  attr_reader :visited

  def initialize(rules, max_width, max_moves)
    @rules = create_reverse_mapping(rules)
    @visited = {}
    @max_width = max_width
    @max_moves = max_moves
    populate()
  end

  def find_solution(row)
    move_data = @visited[row]
    return nil if move_data == nil

    result = []
    n = 0
    while move_data != nil and n < @max_moves
      n += 1
      move = {
        GS_PLAY_IDX      => move_data[S_MOVE_IDX],
        GS_PLAY_PAT      => move_data[S_MOVE_PAT],
        GS_PLAY_REPL     => move_data[S_MOVE_REPL],
        :result          => move_data[S_MOVE_RESULT],
        :num_moves       => move_data[S_MOVE_NUM_MOVES],
        GS_PLAY_RAW_PAT  => move_data[S_MOVE_RAW_PAT],
        GS_PLAY_RAW_REPL => move_data[S_MOVE_RAW_REPL],
        GS_PLAY_CAPTURES => move_data[S_MOVE_CAPTURES],
      }
      result << move # [idx, from, to, next, moves]
      move_data = @visited[move_data[3]]
    end
    return result
  end

private

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
        puts("*** cur_row: #{cur_row}")
        # over every rule...
        @rules.each do |from, to_list|
          next_idx = 0

          # for every location in the current row where this rule applies...
          while (idx = cur_row.index(from, next_idx)) != nil
            next_idx = idx + 1
#            puts("  rule: #{from} matches at #{idx}")
            # apply every replacement...
            to_list.each do |to|
#              puts("    -> applying #{to} at #{idx}")
              # ensuring result fits
              if to.size - from.size <= width_remain
                newrow = cur_row[0...idx] + to + cur_row[idx + from.size..]

                #... and we haven't seen this position before
                # (because if we have, then seeing it again MUST mean we
                # are on a worse/longer path than before)
#                puts("      Newrow: #{newrow}")
                if not @visited.has_key?(newrow)
#                  puts("        NOT VISITED!  Enqueuing")
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
                  @visited[newrow] = [
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


rules= {
  "a"   => ["bb", "cc"],
  "c"   => ["ba", "ab"],
  "aa"  => ["c"],
  "bcb" => [""]
}

rules= {
  "abcd" => [""],
  "a" => ["bc"],
  "bc" => ["bcd", "c"],
  "d" => ["a", "db"],
  "db" => ["b"],
  "cbc" => ["ab"]
}


solver = Solver.new(rules, 7, 18)

solution = solver.find_solution("bbd")

if solution != nil
  solution.each do |move|
    puts("#{move}")
  end
else
  puts "No solution found"
end
