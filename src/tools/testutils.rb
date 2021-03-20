require "test/unit"
require_relative "utils.rb"

class TestUtils < Test::Unit::TestCase

  def test_gcs__all_equal_0()
    gcs = greatest_common_sequence([], [])
    assert_equal([], gcs)
  end

  def test_gcs_all_equal_nonempty()
    a = [1,2,3,4,5]
    b = [1,2,3,4,5]
    gcs = greatest_common_sequence(a, b)
    assert_equal([1,2,3,4,5], gcs)
  end

  def test_gcs_left_n_equal()
    a = [1,2,3,4,5,6]
    b = [1,2,3,4,5,7,8,9,10]
    gcs = greatest_common_sequence(a, b)
    assert_equal([1,2,3,4,5], gcs)
  end

  def test_gcs_right_n_equal()
    a = [4,2,5,1,2,3,4,5]
    b = [0,1,2,3,4,5]
    gcs = greatest_common_sequence(a, b)
    assert_equal([1,2,3,4,5], gcs)
  end

  def test_gcs_strings
    a = "abcDEFGhijkl".chars
    b = "DEFGHIJKL".chars
    gcs = greatest_common_sequence(a, b)
    assert_equal("DEFG".chars, gcs)
  end

  def test_gcs_strings_ci_block
    a = "abcDEFGhijkl".chars
    b = "DEFGHIJKL".chars
    gcs = greatest_common_sequence(a, b) do |a,b|
      a.upcase == b.upcase
    end
    assert_equal("DEFGHIJKL".chars, gcs.join.upcase.chars)
  end

end
