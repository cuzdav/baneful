require 'ruby2d'
require_relative '../custom_wigits.rb'

set title: "Hello Triangle"

class MoveNumberProvider
  def initialize()
    @value = 0
  end

  def cur_move_number()
    return @value
  end

  def next
    @value += 1
  end
end

$movenum = MoveNumberProvider.new

$rot_colors3 = RotatingColorsWigit.new($movenum, 'blue', 'green', 'red')
$rot_colors3.x = 100
$rot_colors3.y = 200
$rot_colors3.height = 30
$rot_colors3.width = 80

$rot_colors2 = RotatingColorsWigit.new($movenum, 'blue', 'green')
$rot_colors2.x = 300
$rot_colors2.y = 200
$rot_colors2.height = 30
$rot_colors2.width = 80


Window.on :key_up do |event|
  $movenum.next()
  $rot_colors2.modified
  $rot_colors3.modified
end

show
