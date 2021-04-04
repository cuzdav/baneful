
require_relative '../game_data.rb'
require_relative '../solve2.rb'
require_relative 'utils.rb'

$verbose = false

require_relative("../config.rb")

$row_num = 0
$level_num = 0
all = true
levels = LEVELS

ARGV.each do |arg|
  case arg
  when /--verbose/
    puts("VERBOSE")
    $verbose = true

  when /--level=([\d]+)/
    all = false
    levels = [LEVELS[$1.to_i]]

  when /--levels?=(.*)/
    all = false
    pat = Regexp.new($1)
    levels = LEVELS.filter{|level| level[:name] =~ pat}

  when /--row=([\d]+)/
    all = false
    row = $1.to_i
  end
end


def solve_level(level, level_num)
  rules = level[:rules]
  rows = level[:rows]
  num_moves = level[:num_moves] || 99
  max_width = level[:max_width] || 9
  max_depth = level[:max_depth] || 15
  type_overrides = level[:type_overrides]
  goal = level[:goal] || ""

  puts("*** Level #{level_num}") if $verbose
  puts("    RULES: #{rules}") if $verbose
  solver = Solver.new(rules, num_moves, max_width)
  $row_num = 0
  solutions = []
  rows.each do |line|
    puts("  Row: #{line}") if $verbose
    $tested += 1
    game_data = GameState.new(
      rules,
      line,
      type_overrides,
      max_width,
      goal)
    game_data.max_depth = max_depth

    solution = solver.find_solution(game_data)
    solutions << solution
    if solution == nil
      puts("\nFailed to find solution for level num #{level_num}: line=\"#{line}\", " + 
	   "row=#{$row_num}, rules:\n#{rules}\n")
      $failed += 1
    end
    $row_num += 1
  end
  return solutions
end

def analyze_level_solutions(level, solutions)
  # total work in progress
  # let's find some basic comparison metrics for each row in a level:
  # * total num of positions that are the same in both solutions
  # * % overlap of positions in both solutions
  # * total num of unique positions (of all solutions for all rows in lvl)
  # * total % of unique positions (of all solutions for all rows in lvl)
  #
  # Terminology:
  # rules: available transformations from a given pattern to one or more
  #        replacement patterns
  # rows: a list of starting positions.  Each row is played individually.
  # level: the combo of one set of rules and a set of rows played with them
  # move: a single play making a transformation.  It's the application of
  #       a rule to some subpattern of a row
  # solution: sequence of moves to solve the row

  all_positions = Hash.new(0)   # every position for every row

  level[:rows].each {|row| all_positions[row] += 1 }
  solutions.each do |solution|
    solution.each do |move|
      pos = move[GS_PLAY_RESULT]
      all_positions[pos] += 1
    end
  end

  stats = []
  solutions.combination(2).each do |a, b|
    lca = greatest_common_sequence(a, b) do |lhs_move, rhs_move|
      lhs_move[GS_PLAY_RESULT] == rhs_move[GS_PLAY_RESULT]
    end
  end

end

def process_level(level)
  level_begin_time = Time.now
  solutions = solve_level(level, $level_num)
  analyze_level_solutions(level, solutions)
  $level_num += 1
  level_end_time = Time.now
  puts("Processing level took #{level_end_time - level_begin_time}")
end

def test_all_levels(levels)
  levels.each do |level|
    process_level(level)
  end
end


$tested = 0
$failed = 0
test_all_levels(levels)
puts("Done: total: #{$tested}, failed: #{$failed}")


