# finds gcs of two enumerable ranges a and b, returning the
# longest contiguous range of equal elements.  If there is a
# tie, it will return one of them.  Unspecified which.
# By default uses == for comparison, but will yield to a block
# if one is provided.
def greatest_common_sequence(a, b)
  best_a_idx = nil
  best_length = 0
  data = Array.new(b.size+1) {|idx| Array.new(a.size+1, 0) }
  b.each_with_index do |b_elt, b_idx|
    a.each_with_index do |a_elt, a_idx|
      equal = block_given? ? (yield(a_elt, b_elt)) : a_elt==b_elt
      if equal
        len = data[b_idx][a_idx] + 1
        data[b_idx+1][a_idx+1] = len
        if len > best_length
          best_length = len
          best_a_idx = a_idx
        end
      end
    end
  end
  return best_length == 0 ? [] : a[best_a_idx - best_length + 1..best_a_idx]
end


