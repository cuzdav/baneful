require 'json'
require_relative 'input_state.rb'
require_relative 'level.rb'
require_relative 'ruleui.rb'

LEVEL_NAME = "name"                     # string
LEVEL_RULES = "rules"                   # hash <string> -> <array-of-string>
LEVEL_ROWS = "rows"                     # <array-of-string>
LEVEL_TYPE_OVERRIDES = "type_overrides" #<json - see README>
LEVEL_NO_VERIFY_ROWS = "no_verify"      #true/false
LEVEL_MAX_ROWS = "max_rows"
LEVEL_MAX_COLS = "max_cols"
LEVEL_NUM_MOVES = "num_moves"
LEVEL_INACTIVE_ROW_OPACITY = "inactive_row_opacity"

# responsible for dealing with filesystem and loading level files,
# and picking next levels, etc.  Skeleton at the moment.


class LevelManager
  attr_reader :curlevel_config

  def initialize(current_directory, initial_level)
    @initial_level = initial_level
    @levels_dir = "#{current_directory}/../resource/levels"
  end

  def start(filename=nil)
    file_list = Dir.entries(@levels_dir).select {|file| file =~ /.*\.json/}
    puts "FILE LIST: #{file_list}"
    @current_levels = open_level_file(file_list[0])
    @level_num = resolve_first_level
    next_level()
  end


  def next_level()
    puts ("***** NEW LEVEL *****")
    curlevel_config = @current_levels[@level_num]
    if curlevel_config != nil
      puts("LEVEL NOW: #{@level_num}: #{@level_name}")
      @level_num += 1
      play_level(curlevel_config)
    end
  end

  def play_level(level_cfg)
    raise "invalid cfg" if not level_cfg
    @curlevel_config = level_cfg
    @level_name = @curlevel_config[LEVEL_NAME]

    @row_num = 0
    if @curlevel != nil
      @curlevel.ruleui.clear
      @curlevel.grid.unhighlight_background
    end

    @curlevel = Level.new(
      self,
      curlevel_config[LEVEL_NUM_MOVES] || 9999,
      curlevel_config[LEVEL_MAX_ROWS] || MAX_ROWS,
      curlevel_config[LEVEL_MAX_COLS] || MAX_COLS,
      20, 20,
      Window.get(:width)-20, playarea_height() #x2, y2
    )
    $input_state.prepare_next_level(
      @curlevel.ruleui,
      @curlevel,
      @curlevel.solver)
  end

  private

  def resolve_first_level
    n = 0
    if @initial_level =~ /^(\d+)$/
      n = @initial_level.to_i
    else
      @current_levels.each do |level_cfg|
        name = level_cfg["name"]
        if name == @initial_level or name =~ /#{@initial_level}/
          break
        end
        n += 1
      end
    end

    return n
  end

  def open_level_file(filename)
    File.open(File.join(@levels_dir, filename)) do |file|
      data = file.read
      return JSON.parse(data)["levels"]
    end
  end
end

