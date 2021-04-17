# frozen_string_literal: true

require 'ruby2d'
require_relative 'custom_widgets'

class CellFactory
  def initialize(level_cfg, color_map, move_number_provider)
    @level_cfg = level_cfg
    @color_map = color_map
    @type_map = {}
    @init_args_map = {}
    @color_map.each_key do |ch|
      @type_map[ch] = Rectangle
      @init_args_map[ch] = [color: color_map[ch]]
    end

    type_overrides = level_cfg[LEVEL_TYPE_OVERRIDES]
    init_type_overrides(type_overrides, move_number_provider) unless type_overrides.nil?
  end

  def get_type_for_char(ch)
    @type_map[ch]
  end

  def create(ch)
    args = @init_args_map[ch]
    klass = get_type_for_char(ch)
    raise("unknown char in level: #{ch}") if klass.nil?

    cell = klass.new(*args)
    cell.opacity = 1
    cell
  end

  private

  def init_type_overrides(type_overrides, move_num_provider)
    type_overrides.each do |override_ch, override_ch_config|
      typename = override_ch_config['type']
      type =
        case typename
        when 'Rectangle'
          Rectangle

        when 'RotatingColors'
          cycle_chars = override_ch_config['cycle_chars']
          if cycle_chars.nil?
            raise "Missing 'cycle_chars' from type_override for " \
                  "#{override_ch} in level: #{@level_cfg[LEVEL_NAME]}\n\t==>#{override_ch_config}"
          end
          # provide the move provider and all the colors it should cycle through
          args = [move_num_provider]
          cycle_chars.each_char do |ch|
            args << @color_map[ch]
          end
          @init_args_map[override_ch] = args
          RotatingColorsWidget

        else
          raise "Unhandled type #{typename}"
        end
      puts("Setting type of '#{override_ch}' to #{type}")
      @type_map[override_ch] = type
    end
  end
end
