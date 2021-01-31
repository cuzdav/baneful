# baneful
block removing logic game

An idea that has floated in my head for a while, but finally decided to make it and see if it was actually fun or not.  :)

Idea is this:

Levels are made of a series of rows of blocks.
You must clear all the rows before moving to the next block.
Only one row is active at a time.  It must be fully cleared before the next row is active.

Rows are made of blocks, each with a color.

At the bottom of the screen are some number of allowable substitution rules.
For a given rule, it has a pattern, and below it is one or more allowed replacements.

If the rule's patten appears in the currently active row in the play area, it can be
replaced by any of the replacements.  Some replacements may change colors and/or numbers
of blocks associated.

Example:

(imagine each character a different color in the game)

ROW: aba

RULE: b -> a

If we apply the rule, we change the row to "aaa"  (b becomes a)

* If the RULE is "ab -> a" and the row is "aba", then applying it would result in "aa"
* If the RULE is "ab -> " and the row is "aba", then applying it woul result in "a" (ab goes away).

In the game, "aba" would be blocks "blue, red, blue", etc.  
The empty string would be a circle with a line through it, like the symbol for "empty set"


This game was inspired by parser theory and BNF grammar.  While it may sound dull to some, I found
joy in the type of thinking that they required, and I tried to translate it into a game format that
was approachable to people without a computer science background.


To Play:

* click on the replacement you would like to apply
* click on the row where you'd like to apply it
Highlights will show valid options, and will darken rules that do not apply, or replacements that do not apply.
(Invalid moves are disabled, such as trying to make the game row wider than the grid allows.)

Plan ahead to avoid going down dead ends.

Keys:

'p' : pause/unpause the music

'+' '-' : volume up / down

']' : next track

'u' : undo move (may ba applied many times until the row is in its initial position)

'r' : restart the row from the beginning

'h' : hint - It will take your current position and give you the next move for an optimal solution.
      _If_ one exists.  Hints are shown incrementally, to not give too much away:
      * 1) first hint shows the rule but does not show you the replacement to use for that rule
         unless only one is possible
      * 2) the next hint shows the replacement row you should select
      * 3) the next hint shows the target, effectively giving you the entire from/to information
      * 4) yet another hint (for the truly lazy) will play the move for you
      However, if there is only one replacement, it's shown along with the rule, and if there's only one
      target, then that will be shown with the replacement, to avoid need for gratuitious clicking for 
      obvious choices.


Game can get challenging, but the goal is to relax and enjoy a somber, meditative mood.  The aim is a feeling
of quiet solitude, as if in a snowstorm in a cabin, with nowhere to go.

TODO:
I am thinking about making a generator for levels for infinite play, with controlled, slow incremental difficulty.
