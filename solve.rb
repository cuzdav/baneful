require_relative 'gamestate'

class Solver

  def initialize()
    @visited = {}
  end

  # return list of moves for ideal solution
  # moves are in the form of tuples returned from game_state.played_moves
  # [offset, to, from] relative to string form of solution.
  def find_solution(game_state)
    @best_solution = nil
    @game_state = game_state.clone_from_cur_position()
    @visited.clear
    @move_number = 0
    find_solution_impl()
    return @best_solution
  end

private

  def find_solution_impl()
    if @game_state.solved?
      moves = @game_state.played_moves
      if @best_solution == nil or moves.size < @best_solution.size
        @best_solution = moves.clone
        @game_state.max_depth = @best_solution.size
      end
      return
    end

    prev_move_num = @visited[@game_state.cur_row]
    if prev_move_num != nil and prev_move_num <= @move_number
      return
    end
    @visited[@game_state.cur_row] = @move_number

    moves = @game_state.possible_plays
    moves.each do |move|
      if @game_state.make_move(*(move[0..2]))
        @move_number += 1
        find_solution_impl()
        @move_number -= 1
        @game_state.undo_move()
      end
    end
  end
end

