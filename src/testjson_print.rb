require "test/unit"
require 'json'
require_relative "json_print.rb"

class TestJsonPrint < Test::Unit::TestCase

  def setup()
    @json = JsonPrint.new
  end

  def test_number
    result = JsonPrint.new.encode(1729)
    assert_equal("1729", result)
  end

  def test_string
    result = @json.encode("abcdefg")
    assert_equal("\"abcdefg\"", result)
    JSON.parse(result)
  end

  def test_array1
    result = @json.encode([1, 2, 3])
    assert_equal("[\n    1,\n    2,\n    3\n]", result)
    JSON.parse(result)
  end

  def test_array2
    result = @json.encode([[1, 2, 3]])
    assert_equal("[\n    [\n        1,\n        2,\n        3\n    ]\n]", result)
    JSON.parse(result)
  end

  def test_array3
    # after a nesting level of 2, they should be on a single line
    # this is the main reason for this custom json code
    result = @json.encode([[[1, 2, 3]]])
    assert_equal("[\n    [\n        [1, 2, 3]\n    ]\n]", result)
    JSON.parse(result)
  end

  def test_string_internal
    width = 11
    result = @json.send(:jstr_string, "abcdefg", width)
    assert_equal("  \"abcdefg\"", result)
    JSON.parse(result)
  end

  def test_hash1
    @json.keywidth = 5
    result = @json.encode({"a" => 123, "b" => 234})
    expect = <<JSON
{
      "a": 123,
      "b": 234
}
JSON
    assert_equal(expect.strip, result)
    JSON.parse(result)
  end

  def test_level
    @json.keywidth = 5
    result = @json.encode(
      {
        name: "choice",
        rules: {
          "ab" => ["a", "b", "c"],
          "ba" => ["b", "c", "a"],
          "c" => [""]
        },
        rows: [
          "abba",
        ]
      })
    expect = <<JSON
{
    "name": "choice",
    "rows": [
        "abba"
    ],
    "rules": {
         "ab": ["a", "b", "c"],
         "ba": ["b", "c", "a"],
          "c": [""]
    }
}
JSON
    assert_equal(expect.strip, result)
    JSON.parse(result)
  end


end

