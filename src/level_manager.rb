require 'json'
require_relative 'input_state.rb'
require_relative 'level.rb'
require_relative 'ruleui.rb'

LEVEL_KEY_NAME = "name"
LEVEL_KEY_RULES = "rules"
LEVEL_KEY_ROWS = "rows"
LEVEL_KEY_TYPE_OVERRIDES = "type_overrides"


# responsible for dealing with filesystm and loading level files,
# and picking next levels, etc.

class LevelManager
  attr_reader :curlevel_config

  def initialize(current_directory, initial_level)
    @initial_level = initial_level
    @levels_dir = "#{current_directory}/../resource/levels"
  end

  def start
    file_list = Dir.entries(@levels_dir).select {|file| file =~ /.*\.json/}
    puts "FILE LIST: #{file_list}"
    @current_levels = open_level_file(file_list[0])
    @level_num = resolve_first_level
    next_level()
  end

  def next_level()
    puts ("***** NEW LEVEL *****")
    @curlevel_config = @current_levels[@level_num]
    @level_name = @curlevel_config[LEVEL_KEY_NAME]
    if @curlevel_config != nil
      puts("LEVEL NOW: #{@level_num}: #{@level_name}")
      @level_num += 1
      @row_num = 0

      if @curlevel != nil
        @curlevel.ruleui.clear
        @curlevel.grid.unhighlight_background
      end

      @curlevel = Level.new(
        self,
        @curlevel_config["num_moves"] || 9999,
        MAX_ROWS,
        @curlevel_config["max_width"] || MAX_WIDTH,
        20, 20, #playarea_height() - @curlevel_config["rows"].size * 50,
        Window.get(:width)-20, playarea_height() #x2, y2
      )
      $input_state.prepare_next_level(
        @curlevel.ruleui,
        @curlevel, @curlevel.solver)
    end
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

