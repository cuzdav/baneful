
FILES=$(ls *.rb | grep -Ev 'main.rb|verify_levels.rb' | grep -v 'test.*\.rb')
cat $FILES main.rb > allsource.rb
sed -i'' allsource.rb -e '/require_relative/d'

ruby2d build --native allsource.rb

./build/app
