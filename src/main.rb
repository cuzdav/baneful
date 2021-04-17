# frozen_string_literal: true

require 'ruby2d'
require_relative 'input_state'
require_relative 'level'
require_relative 'music_player'
require_relative 'config'

Window.set title: 'Baneful'
Window.set({ width: 1024, height: 768 })

MAX_ROWS = 5
MAX_COLS = 9

VERT_RULE_OFFSET_PX = 30
HORIZ_RULE_OFFSET_PX = 5

XCOLORS = %w[
  aqua red lime fuchsia
  silver purple gray yellow
  olive blue green
  orange brown maroon
  navy white black teal
].freeze

ELECTRIC_BLUE   = Color.new('#A0FFFF')
FERRARI_RED     = Color.new('#F15015')
MALACHITE_GREEN = Color.new('#19FF40')
HARVEST_GOLD    = Color.new('#EEBBAA')
BRIGHT_LAVENDER = Color.new('#BB99FF')
PASTEL_YELLOW   = Color.new('#FFFF80')

COLORS = [
  ELECTRIC_BLUE,
  FERRARI_RED,
  MALACHITE_GREEN,
  HARVEST_GOLD,
  BRIGHT_LAVENDER,
  PASTEL_YELLOW
].freeze

def playarea_height
  Window.get(:height) / 3
end

def rulearea_max_height
  Window.get(:height) / 2
end

initial_level = nil
ARGV.each do |arg|
  initial_level = Regexp.last_match(1) if arg =~ /--level=(.*)/
end

current_directory = File.dirname($PROGRAM_NAME)
music = MusicPlayer.new(current_directory, MUSIC)
level_manager = LevelManager.new(current_directory)
input_state = InputState.new(music, level_manager, initial_level)
level_manager.input_state = input_state

Window.on :mouse_move do |event|
  input_state.on_mouse_move(event)
end
Window.on :mouse_down do |event|
  input_state.on_mouse_down(event)
end
Window.on :mouse_up do |event|
  input_state.on_mouse_up(event)
end
Window.on :key_up do |event|
  input_state.on_key_up(event)
end
Window.on :key_down do |event|
  input_state.on_key_down(event)
end

input_state.startup('title_screen.json')

tick = 0
update do
  tick += 1
  if tick == 60
    music.update
    tick = 0
  end
  input_state.update
end

show
