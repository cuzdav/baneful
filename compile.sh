
# probably you shouldn't use this.  It's barely more than a thought experiment
# and requires mruby compiler installed to work.  And it's out of date, but I
# keep it as an idea for myself.

FILES=$(ls *.rb | grep -Ev 'main.rb|verify_levels.rb' | grep -v 'test.*\.rb')
cat $FILES main.rb > allsource.rb
sed -i'' -e '/require_relative/d' allsource.rb 

ruby2d build --native allsource.rb

./build/app
