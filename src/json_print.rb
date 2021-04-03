
# My own JSON code?  Yeah, I wanted it formatted in a specific way and could not
# get the standard code to do it that way.  If you know how to make the standard
# JSON class output it in this format, please share. :)

class JsonPrint
  attr_accessor :keywidth

  def encode(obj)
    @depth = 0
    @out = ""
    jstr_object(obj)
    return @out
  end

  def initialize()
    @keywidth = 5
    @depth = 0
    @out = ""
  end

  private

  def indent_str
    "    " * @depth
  end

  def indent
    @out += indent_str
  end

  def newline
    @out += "\n"
  end

  def iputs(msg)
    indent
    @out += msg
  end

  def jstr_object(obj)
    case obj
    when Array
      jstr_array(obj)
    when String
      jstr_string(obj)
    when Symbol
      jstr_string(obj.to_s)
    when Hash
      jstr_hash(obj)
    else
      @out += obj.to_s
    end
  end

  def jstr_string(str, width=0)
    qstr = '"' + str.to_s + '"'
    @out += ("%*s" % [width, qstr])
  end

  def jstr_hash(hash)
    @out += "{"
    @depth += 1
    needsep = false
    hash.keys.sort.each do |k|
      v = hash[k]
      if needsep
        @out += ','
      else
        needsep = true
      end
      newline
      indent
      jstr_string(k, @keywidth)
      @out += ': '
      jstr_object(v)
    end
    @depth -= 1
    newline
    iputs("}")
  end

  def jstr_array(ary)
    @out += '['
    @depth += 1
    multi_line = @depth < 3
    if multi_line
      newline
      indent
      sep = ",\n" + indent_str
    else
      sep = ', '
    end

    first = true
    ary.each do |elt|
      @out += first ? "" : sep
      first = false

      jstr_object(elt)
    end
    @depth -= 1
    if multi_line
      newline
      indent
    end
    @out += ']'
  end
end

