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
#   letters represent blocks, [a-p] are solid colors, '.' is a wildcard
#   In replacements, 1,2,3,...9 are captures of whatever match corresponded to the nth previous '.'
# rows: array of strings, representing the play-area game rows
# The 0th element in this array is at the TOP of the screen, while the
# last element in the array is the FIRST row active in the play area.
# That is, the are drawn on the screen approximately how they look here
# (but centered.)
# All characters appearing in rows must appear at least once somewhere in rules
# characters can be anything, but the same character refers to the same color block.
# Each unique character is a different color.
#
# You should be able to quickly match each set of rules/rows to in-game screens and
# can add your own.

LEVELS =[
  ## for testing...
  ##{
  ##  rules: {
  ##    "cba" => [""],
  ##    "..." => ["321"],
  ##  },
  ##  rows: [
  ##    "abc",
  ##  ]
  ##},

  {
    name: "hello",
    rules: {
      "a" => [""],
    },
    rows: [
      "aaa",
      "a"
    ]
  },
  {
    name: "2-step",
    rules: {
      "a"  => ["b"],
      "b" => [""],
    },
    rows: [
      "a",
    ]
  },
  {
    name: "simple chain",
    rules: {
      "a" => ["b"],
      "b" => ["c"],
      "c" => [""]
    },
    rows: [
      "a",
    ]
  },

  {
    name: "pairs",
    rules: {
      "ab" => [""],
      "ba" => [""],
    },
    rows: [
      "abba",
    ]
  },

  {
    name: "choice",
    rules: {
      "ab" => ["a", "b", "c"],
      "c" => [""]
    },
    rows: [
      "abba",
    ]
  },

  {
    name: "setup",
    rules: {
      "ab" => [""],
      "ba" => ["ab"],
      "b" => ["a"],
    },
    rows: [
      "baab",
      "bbbb",
      "baba",
    ]
  },

  {
    name: "reduce",
    rules: {
      "ab"  => ["b"],
      "b" => [""],
    },
    rows: [
      "aaab",
    ]
  },
  {
    name: "rows",
    rules: {
      "a" => ["bb"],
      "aab" => ["ab", "aaa"],
      "bbb" => [""],
    },
    rows: [
      "aaaaabb",
      "ab",
      "ba",
    ]
  },
  {
    name: "careful",
    rules: {
      "b"  => ["bb", ""],
      "abbb" => [""],
    },
    rows: [
      "aab",
      "bab",
      "b"
    ]
  },
  {
    name: "cavity",
    rules: {
      "bb"  => [""],
      "aa" => [""],
    },
    rows: [
      "babaabab",
      "abbbbbba",
    ]
  },
  {
    name: "rise and fall",
    rules: {
      "a"  => ["cc"],
      "cbc" => ["b"],
      "b" => [""]
    },
    rows: [
      "aba",
    ]
  },
  {
    name: "reverse",
    rules: {
      "a"    => ["ba", "bb"],
      "ba"   => ["aab"],
      "abbb" => [""]
    },
    rows: [
      "a",
    ]
  },

  {
    name: "onion",
    rules: {
      "a"  => ["aa", "ba"],
      "aba" => [""],
      "b" => ["ab"]
    },
    rows: [
      "baa",
      "a",
      "ba",
      "aa",
      "aba",
    ]
  },

  {
    name: "patchwork",
    rules: {
      "a"  => ["b", "c"],
      "b" => ["ac"],
      "cc" => [""]
    },
    rows: [
      "abc",
      "ac",
      "a",
    ]
  },

  {
    name: "reverence",
    rules: {
      "a"    => ["ba", "bb"],
      "ba"   => ["ab"],
      "abbbb" => [""]
    },
    rows: [
      "a",
    ]
  },

  {
    name: "parity",
    rules: {
      "aa"  => ["ab"],
      "bb" => ["b"],
      "ab" => [""]
    },
    rows: [
      "aaaaaaa"
    ]
  },

  {
    name: "aha!",
    rules: {
      "a"  =>  ["b"],
      "ab"  =>  ["bc"],
      "dbc" => ["db", ""],
    },
    rows: [
      "daaaaa"
    ]
  },


  #KINDA HARD
  {
    name: "I believe in you",
    rules: {
      "a"   => ["bb", "cc"],
      "c"   => ["ba", "ab"],
      "aa"  => ["c"],
      "bcb" => [""]
    },
    rows: [
      "bcbc",
      "c",
      "a", #"cc" "bac" "baab" "bcb"
    ]
  },

  {
    name: "April",
    rules: { # a/bc/bcd/bcdd/bcda/bcdbc/bcbc/cbc/aa/
      "abcd" => [""],
      "a" => ["bc"],
      "bc" => ["bcd", "c"],
      "d" => ["a", "db"],
      "db" => ["b"],
      "cbc" => ["ab"]
    },
    rows: [
      "bd",
      "acd"
    ]
  },

## This level works but is not visibly displayed in a useful way yet
##{
##  name: "reverse 3",
##  rules: {
##    "cba" => [""],
##    "..." => ["321"],
##  },
##  rows: [
##    "abc",
##  ]
##},
##
  {
    name: "wild!",
    rules: {
      "." => [""],
    },
    rows: [
      "abcd",
    ]
  },

  {
    name: "I remember you",
    rules: {
      "b" => [""],
      "a.a" => ["1", ""],
    },
    rows: [
      "aabaa",
    ]
  },

]
