require_relative 'gamestate'

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
      result << move_data # [idx, from, to, next, moves]
      move_data = @visited[move_data[3]]
    end
    return result
  end

private

  def populate()
    # use double buffering to dequeue from one buffer and enqueue onto anothre,
    # then swap when we run out.  This allows for tracking the depth.
    q = []
    next_q = [""]
    move_count = 0

    while move_count <= @max_moves and not next_q.empty?
      move_count += 1
      q = next_q
      next_q = []
      while not q.empty?
        cur_row = q.pop
        width_remain = @max_width - cur_row.size
        @rules.each do |from, to_list|
          idx = 0
          while (idx = cur_row.index(from, idx)) != nil
            to_list.each do |to|
              if to.size - from.size <= width_remain
                newrow = cur_row[0...idx] + to + cur_row[idx + from.size..]
                if not @visited.has_key?(newrow)
                  next_q.push(newrow)
                  # Note, to/from are reversed because we create them with a
                  # reverse map (to build up positions from an empty board), but
                  # we enqueue the moves that should be done to move closer to
                  # the solution.
                  @visited[newrow] = [idx, to, from, cur_row, move_count]
                end
              end
            end
            idx += 1
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
