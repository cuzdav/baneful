require 'ruby2d'
require_relative 'solve2'
require_relative 'ruleui'
require_relative 'game_data'
require_relative 'level'

class Hint
  attr_reader :solution

  def initialize(input_state, solver)
    @input = input_state
    @solver = solver
    @target_hintbox = Rectangle.new(
      color: 'white',
      z: 0,
      opacity: 0.8)
    clear
  end

  def next_hint(cur_level)
    puts("*** next hint: strength#{@hint_strength}")
    case @hint_strength
    when 0
      rule_hint(cur_level)
    when 1
      repl_hint(cur_level)
    when 2
      target_hint(cur_level)
    when 3
      @input.apply_choice_maybe
    else
      raise 'unreachable'
    end
  end

  def clear
    @solution = nil
    @rule.unselect_hints unless @rule.nil?
    @target_hintbox.remove
    @hint_strength = 0
  end

  private

  def rule_hint(cur_level)
    game_data = cur_level.game_data

    # find the next move to make...
    @solution = @solver.find_solution(game_data)

    if !@solution.empty?
      @next_move = @solution[0]
      rpat, rrepl = game_data.get_raw_rule_and_repl_for_move(@next_move)
      @rule = cur_level.ruleui.get_rule_for_pat_and_repl(rpat, rrepl)
      if @rule
        @input.select_rule(@rule)

        # if rule has only one replacement, then skip the rule
        # hint and go straight to the replacement hint
        if @rule.replacement_strs.size == 1
          repl_hint(cur_level)
        else
          @rule.show_pattern_hint
        end
        @hint_strength += 1
      end
    else
      puts('***** no @solution found *****')
    end
  end

  def repl_hint(cur_level)
    puts ("[repl_hint] next move: #{@next_move}")
    puts ("rule: #{@rule.from_str} -> #{@rule.replacement_strs}")
    repl = @next_move[GS_PLAY_RAWREPL]

    idx = @rule.index_of_repl(repl)
    raise "could not find index of repl #{repl}" if idx == nil
    @rule.show_repl_hint(repl)
    @hint_strength += 1
    @input.select_replacement(idx + FIRST_REPL_ROW)

    # count unique targets (to-string and offset)
    # only one move, so show where it should go at once
    targets = {}
    cur_level.game_data.possible_plays.each do |idx, from, to|
      targets[idx.to_s + from] = 1 if to == repl
    end
    if targets.size() == 1
      target_hint(cur_level)
    end
  end

  def target_hint(cur_level)
    grid = cur_level.grid

    # grid-coord from string coord is offset by eff_col
    idx = @next_move[GS_PLAY_IDX] + cur_level.eff_col
    pat = @next_move[GS_PLAY_PAT]
    end_idx = idx + pat.size - 1
    x = cur_level.active_x1_coord(idx)
    y = cur_level.active_y1_coord()
    width = cur_level.active_x2_coord(end_idx) - x
    height = grid.cell_height + grid.vert_gap_px

    @target_hintbox.x = x
    @target_hintbox.y = y
    @target_hintbox.width = width
    @target_hintbox.height = height
    @target_hintbox.add
    @hint_strength += 1
    @input.select_target(idx, @next_move)
  end

end
