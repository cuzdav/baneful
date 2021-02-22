require 'ruby2d'
require_relative 'input_state.rb'
require_relative 'level.rb'
require_relative 'music_player.rb'

# <levels> is an array of <LEVEL>
# where <LEVEL> is [<RULES>, <ROWS>]
# and <RULES> is hash <from> => [<REPLACEMENTS>]
# and <ROWS> is array of starting strings

require_relative 'config.rb'

# index int <levels> described above
$level_num = 0
# current row in current level
$row_num = 0

$curlevel_spec = nil  # description of level
$curlevel = nil       # ui game objects for cur level

charmap = {}

MAX_ROWS = 12
MAX_WIDTH = 9

VERT_RULE_OFFSET_PX = 30
HORIZ_RULE_OFFSET_PX = 5

RULE_AREA_HEIGHT_PX = 150

COLORS = [
  "aqua", "red", "lime", "yellow", "purple", "gray",
  "olive",  "blue", "green",
  "orange","brown", "maroon",
  "navy", "white", "silver", "black", "teal", "fuchsia"]

def playarea_height()
  return Window.get(:height) - RULE_AREA_HEIGHT_PX
end

$music = MusicPlayer.new(MUSIC)

$input_state = InputState.new($music)

Window.on :mouse_move do |event|
  $input_state.on_mouse_move(event)
end
Window.on :mouse_down do |event|
  $input_state.on_mouse_down(event)
end
Window.on :mouse_up do |event|
  $input_state.on_mouse_up(event)
end
Window.on :key_up do |event|
  $input_state.on_key_up(event)
end
Window.on :key_down do |event|
  $input_state.on_key_down(event)
end


def next_level()
  puts ("***** NEW LEVEL *****")
  $curlevel_config = LEVELS[$level_num]
  if $curlevel_config != nil
    puts("LEVEL NOW: #{$level_num}: #{$curlevel_config[:name]}")
    $level_num += 1
    $row_num = 0

    if $curlevel != nil
      $curlevel.ruleui.clear
      $curlevel.grid.unhighlight_background
    end

    $curlevel = Level.new(
      $curlevel_config,
      99999, #num moves
      MAX_ROWS,
      MAX_WIDTH,
      20, 20,
      Window.get(:width) - 20, playarea_height())
    $input_state.prepare_next_level(
      $curlevel.ruleui,
      $curlevel, $curlevel.solver)
  end
end

ARGV.each do |arg|
  if arg =~ /--level=([\d]+)/
    $level_num = $1.to_i
    if $level_num >= LEVELS.size
      puts("Max level is #{LEVELS.size}")
      exit 1
    end
    puts("Starting on level #{$level_num}")
  elsif arg =~ /--level=(.*)/
    pat = Regexp.new($1)
    idx = 0
    LEVELS.each do |lvl|
      break if lvl[:name] =~ pat
      idx += 1
    end
    if idx == LEVELS.size
      puts("Max level is #{LEVELS.size}")
      exit 1
    end
    $level_num = idx
  end
  puts("Starting on level #{$level_num}")
end

next_level()

puts("******* LEVELS: #{LEVELS.size}")

tick = 0

update do
  tick += 1
  if tick == 60
    $music.update
    tick = 0
  end
end

show
