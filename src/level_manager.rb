require 'json'
require_relative 'input_state'
require_relative 'level'
require_relative 'ruleui'

LEVEL_NAME = 'name'.freeze                     # string
LEVEL_RULES = 'rules'.freeze                   # hash <string> -> <array-of-string>
LEVEL_ROWS = 'rows'.freeze                     # <array-of-string>
LEVEL_TYPE_OVERRIDES = 'type_overrides'.freeze # <json - see README>
LEVEL_NO_VERIFY_ROWS = 'no_verify'.freeze      # true/false
LEVEL_MAX_ROWS = 'max_rows'.freeze
LEVEL_MAX_COLS = 'max_cols'.freeze
LEVEL_NUM_MOVES = 'num_moves'.freeze
LEVEL_INACTIVE_ROW_OPACITY = 'inactive_row_opacity'.freeze

# responsible for dealing with filesystem and loading level files,
# and picking next levels, etc.  Skeleton at the moment.


class LevelManager
  attr_reader :curlevel_config
  attr_accessor :input_state

  def initialize(current_directory)
    @input_state = nil
    @levels_dir = "#{current_directory}/../resource/levels"
  end

  def open_level_group(filename, initial_level)
    @current_levels = open_level_file(filename)
    @level_num = resolve_first_level(initial_level)
    prepare_next_level
  end


  def prepare_next_level
    puts('***** NEW LEVEL *****')
    curlevel_config = @current_levels[@level_num]
    return if curlevel_config.nil?

    puts("LEVEL NOW: #{@level_num}: #{@level_name}")
    @level_num += 1
    play_level(curlevel_config)
  end

  def play_level(level_cfg)
    raise 'invalid cfg' unless level_cfg

    @curlevel_config = level_cfg
    @level_name = @curlevel_config[LEVEL_NAME]

    @row_num = 0
    @curlevel.clear unless @curlevel.nil?

    @curlevel = Level.new(
      self,
      curlevel_config[LEVEL_NUM_MOVES] || 9999,
      curlevel_config[LEVEL_MAX_ROWS] || MAX_ROWS,
      curlevel_config[LEVEL_MAX_COLS] || MAX_COLS,
      20, 20,
      Window.get(:width)-20, playarea_height #x2, y2
    )
    @input_state.prepare_next_level(
      @curlevel.ruleui,
      @curlevel,
      @curlevel.solver)
  end

  private

  def resolve_first_level(initial_level)
    n = 0
    if initial_level =~ /^(\d+)$/
      n = initial_level.to_i
    else
      @current_levels.each do |level_cfg|
        name = level_cfg['name']
        break if name == @initial_level or name =~ /#{initial_level}/

        n += 1
      end
    end
    n
  end

  def open_level_file(filename)
    File.open(File.join(@levels_dir, filename)) do |file|
      data = file.read
      return JSON.parse(data)['levels']
    end
  end
end

