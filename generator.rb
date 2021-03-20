require_relative "gamestate.rb"
require_relative "solve2.rb"


def row_generator(mapping, result, seen, maxwidth, row, depth)
  depth += 1
  mapping.each do |from, to_list|
    i = 0
    # find each place where this "from" can go...
    while (i = row.index(from, i))
      # and insert the "to" in each place
      to_list.each do |to|
        if to.size + row.size - from.size <= maxwidth
          # make a replacement (remove <from>, add <to> at idx)
          newrow = row[0...i] + to + row[i + from.size..]
          if seen[newrow] == nil
            seen[newrow] = 1
            result << [depth, newrow]
            row_generator(mapping, result, seen, maxwidth, newrow, depth)
          end
        end
      end
      i += 1
    end
  end
end


def generate(rules, maxwidth)
  mapping = create_reverse_mapping(rules)
  puts("rules: #{rules}")
  puts("rev mapping: #{mapping}")

  result = []
  row_generator(mapping, result, {}, maxwidth, "", 0)
  return result
end


rules= {
    "abcd" => [""],
    "a" => ["bc"],
    "bc" => ["bcd", "c"],
    "cbc" => ["ab"]
  }

rules= {
  "a"   => ["bb", "cc"],
  "c"   => ["ba", "ab"],
  "aa"  => ["c"],
  "bcb" => [""]
}

rules= { # a/bc/bcd/bcdd/bcda/bcdbc/bcbc/cbc/aa/
  "abcd" => [""],
  "a" => ["bc"],
  "bc" => ["bcd", "c"],
  "d" => ["a", "db"],
  "db" => ["b"],
  "cbc" => ["ab"]
}

# The solver is too slow to be used to verify the minimum number of moves.
# However, the depth is a decent approximation, though it can be wrong,
# sometimes by a lot, but it is _super fast_ and a viable option at runtime.
# Running the solver may be too expensive to use while generating levels.


solver = Solver.new(rules, 20, 7)
rows = generate(rules, 4)
annotated_rows = []
rows.each do |depth, row|
  gs = GameState.new(rules, row)
  solution = solver.find_solution(gs)
  annotated_rows << [solution.size, depth, row]
end

annotated_rows.sort!

level = {
  rules: rules,
  rows: rows.reverse[0..2].map{|solution, row| row}
}

annotated_rows.each do |solution, depth, row|
  puts("[#{solution}, #{depth}, #{row}]")
end

puts("------")
puts("#{level},")

