MUSIC = [
  ['sb_ephemera.mp3', 339],
  ['sb_hiraeth.mp3', 347],
  ['sb_neon.mp3', 260],
  ['sb_signaltonoise.mp3', 354],
  ['sb_sleep.mp3', 185],
  ['sb_solace.mp3', 357],
  ['sb_undertow.mp3', 250]
]


test = {
    name: "test",
    rules: {
      "abcdef" => ["aaa"],
      "aaa" => [""],
      "a" => ["d"],
      "b" => ["e"],
      "c" => ["a"],
    },
    rows: [
      "abcdef",
    ],
    type_overrides: {
      "d" => {
        "type" => "RotatingColors",
        "cycle_chars" => "aba",
      },
      "e" => {
        "type" => "RotatingColors",
        "cycle_chars" => "bac",
      },
    }
  }
