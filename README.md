# baneful
block removing logic game

Installation
------------
see install-ubuntu.sh or install-mac.sh


Play
----
An idea that has floated in my head for a while, but finally decided
to make it and see if it was actually fun or not.  :)


Levels are made of a series of rows of blocks, and rules to transform
them.  You must clear all the cells in one row before moving to the 
next row, and all the rows to move to the next level.

Only the "bottom" row is active, which should be obvious by brightness.

Rows are made of blocks of various color.

At the bottom of the screen are some number of transformations that
can apply to the row to help clear it.  Some tranformations change
colors, some remove blocks, some rearrange and modify the row.  And
some may even add blocks.

It shows a pattern of color blocks, and below it one or more replacement patterns.

Example:

| <RED><RED><RED> |
|-----------------|
| <BLUE><BLUE>    |
| <empty>         |

If the above <RED> and <BLUE> were blocks in the game, the above
"rule" would mean, 
1) "three adjacent reds can be removed and replaced by two blue blocks", OR
2) "three adjacent reds can be removed and replaced with "nothing" (empty).

In the game <Empty> is drawn as a circle with a red circle and a white
line through it, somewhat looking like an empty set.


Example:

(imagine each character a different color in the game.  a=<RED>, b=<BLUE>

ROW: aba

* If the rule is  (b -> a)  (read: "b turns into a"), and the row is "aba", the result is "aaa"

* If the rule is "ab -> a" and the row is "aba", the result is "aa"

* If the rule is "ab -> (empty)" and the row is "aba", the result is "a" ("ab" goes away).

In the actual game, where letters were used here, a color block is drawn.


This game was inspired by parser theory and BNF grammar, hence the
name "Baneful", which includes BNF.  :) While parser theory may not
sound "fun", I found joy in the type of thinking that they required,
and I tried to translate it into a game format that was approachable
to people without a computer science background.  It passed the "kid
test" a few years ago on my 12-year old and her friends.


To Play:

1) click on the replacement you would like to apply

2) click on the row (at the top) where you'd like to apply it.  

   =====> If it matches multiple places in the row, move the mouse
          left/right to change which match is targeted.

* click when to apply the transformation

* If the wrong replacement is selected, right-click (or hit 'ESC') to
unselect.


Highlights will show valid options, and will darken rules that do not
apply, or replacements that do not apply.  (Invalid moves are
disabled, such as trying to make the game row wider than the grid
allows.)

** Plan ahead to avoid going down dead ends **

Keys:

'p' : pause/unpause the music

'+' '-' : volume up / down

']' : next music track

'u' : undo move (may ba applied many times until the row is in its initial position)

'r' : restart the row from the beginning

'h' : hint - It will take your current position and give you the next move for an optimal solution.
      _If_ one exists.  Hints are shown incrementally, to not give too much away:
      * 1) first hint shows the rule to apply for the optimal solution from the current position.  It 
           does not show you which replacement to use or where the rule should apply.
      * 2) the next hint shows the replacement row you should select in the rule
      * 3) the next hint shows the target, effectively giving you the entire from/to information
      * 4) the next hint will actually play the move for you.  Every level
           should be solvable with enough hints and nothing else, but where's the
           fun in that?

      If there is only one replacement for a given rule, then showing the rule to pick is also showing
      the replacement to pick.  As such, it will give both in the "same" hint request, to avoid needing
      pointless key taps.

      Similarly, if there's only one matching target in the row at top, then that will be shown
      with the replacement since it's also obvious and no need to incrementally reveal it.

      (Which implies if there's only one replacement for a rule, and only one match in the row, then a
      single hint will give the full answer in one "go".)

The game can get challenging, but the goal is to relax and enjoy a
somber, meditative mood.  The aim is a feeling of quiet solitude, as
if in a snowstorm in a cabin, with time on your hands and nowhere to go.
(Familiar feeling in a Chicago Pandemic winter.)


TODO: I am thinking about making a generator for levels for infinite
play, with controlled, slow incremental difficulty.  Currently I have
a generator that takes a rule-set as input and generates levels from
it, but it is not connected to the game.  If I can come up with a
"rule generator" too, then I might hook it all together.
