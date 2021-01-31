
require_relative 'gamestate.rb'
require_relative 'solve.rb'

$verbose = false

require_relative("config.rb")

row_num = 0
level_num = 0
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
  max_depth = level[:max_depth] || 20
  goal = level[:goal] || ""

  puts("*** Level #{level_num}") if $verbose
  puts("    RULES: #{rules}") if $verbose
  solver = Solver.new
  row_num = 0
  rows.each do |line|
    puts("  Row: #{line}") if $verbose
    $tested += 1
    gamestate = GameState.new(
      rules,
      line,
      num_moves,
      max_width,
      goal)
    gamestate.max_depth = max_depth

    if solver.find_solution(gamestate) == nil
      puts("\nFailed to find solution for level num #{level_num}: line=\"#{line}\", row=#{row_num}, rules:\n#{rules}\n")
      $failed += 1
    end
    row_num += 1
  end

end

$tested = 0
$failed = 0
$level_num = 0
levels.each do |level|
  solve_level(level, level_num)
  level_num += 1
end
puts("Done: total: #{$tested}, failed: #{$failed}")
