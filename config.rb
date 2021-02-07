MUSIC = [
  ['sb_ephemera.mp3', 339],
  ['sb_hiraeth.mp3', 347],
  ['sb_neon.mp3', 260],
  ['sb_signaltonoise.mp3', 354],
  ['sb_sleep.mp3', 185],
  ['sb_solace.mp3', 357],
  ['sb_undertow.mp3', 250]
]

#
# Format is pretty obvious

# Each level is a dictionary consisting of rules and rows.
#
# rules: a dictionary of PATTERN -> [ARRAY OF REPLACEMENTS]
# rows: array of strings, representing the game rows
# The 0th element in this array is at the TOP of the screen, while the
# last element in the array is the FIRST row active in the play area.
# That is, the are drawn on the screen approximately how they look here
# (Except centered.)
# All characters appearing in rows must appear at least once somewhere in rules
# characters can be anything, but the same character refers to the same color block.
# Each unique character is a different color.
#
# You should be able to quickly match each set of rules/rows to in-game screens and
# can add your own.

LEVELS =[
  {
    rules: {
      "x" => [""],
    },
    rows: [
      "xxx",
      "x"
    ]
  },
  {
    rules: {
      "x"  => ["y"],
      "y" => [""],
    },
    rows: [
      "yxy",
      "xx",
      "y",
    ]
  },
  {
    rules: {
      "x-" => [""],
      "-" => ["x"],
    },
    rows: [
      "x-",
      "--"
    ]
  },
  {
    rules: {
      "xy"  => ["y"],
      "y" => [""],
    },
    rows: [
      "xxxy",
      "yxy",
      "yy"
    ]
  },
  {
    rules: {
      "x" => ["yy"],
      "xxy" => ["xy", "xxx"],
      "yyy" => [""],
    },
    rows: [
      "xxxxxyy",
      "xy",
      "yx",
    ]
  },
  {
    rules: {
      "y"  => ["yy", ""],
      "xyyy" => [""],
    },
    rows: [
      "xxy",
      "yxy",
      "y"
    ]
  },
  {
    rules: {
      "yy"  => [""],
      "xx" => [""],
    },
    rows: [
      "yxyxxyxy",
      "xyyyyyyx",
    ]
  },
  {
    rules: {
      "x"  => ["zz"],
      "zyz" => ["y"],
      "y" => [""]
    },
    rows: [
      "xyx",
    ]
  },
  {
    rules: {
      "x"  => ["xx", "yx"],
      "xyx" => [""],
      "y" => ["xy"]
    },
    rows: [
      "xxyx",
      "xyx",
    ]
  },

  {
    rules: {
      "x"  => ["y", "z"],
      "y" => ["xz"],
      "zz" => [""]
    },
    rows: [
      "xyz",
      "xz",
      "x",
    ]
  },

  {
    rules: {
      "x"  => ["y", "z"],
      "y" => ["xz"],
      "zzz" => [""]
    },
    rows: [
      "xyz",
      "xz",
      "x",
    ]
  },

  {
    rules: {
      "xx"  => ["xy"],
      "yy" => ["y"],
      "xy" => [""]
    },
    rows: [
      "xxxxxxx"
    ]
  },

  {
    rules: {
      "0"  =>  ["1"],
      "01"  =>  ["1x"],
      "-1x" => ["-1", ""],
    },
    rows: [
      "-00000"
    ]
  },


  #KINDA HARD
  {
    rules: {
      "x"   => ["yy", "zz"],
      "z"   => ["yx", "xy"],
      "xx"  => ["z"],
      "yzy" => [""]
    },
    rows: [
      "z",
      "x", #"zz" "yxz" "yxxy" "yzy"
    ]
  },

  # april's first, hard level
  {
    rules: { # a/bc/bcd/bcdd/bcda/bcdbc/bcbc/cbc/aa/
      "abcd" => [""],
      "a" => ["bc"],
      "bc" => ["bcd", "c"],
      "d" => ["a", "db"],
      "db" => ["b"],
      "cbc" => ["ab"]
    },
    rows: [
      "acd"
    ]
  },




]
