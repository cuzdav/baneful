PACKAGES='
ruby-dev
libsdl2-dev
libsdl2-image-dev
libsdl2-mixer-dev
libsdl2-ttf-devmruby
'

# if you intend to run the compile script (at this point it doesn't really do much useful)
# it will compile the code to a bundled executable with mruby, so also install this:
# libmruby-dev

sudo apt install $PACKAGES

# download these songs to play the full soundtrack.  You can of course
# play your own (edit config.rb) but be sure to update the duration.

MUSIC='
https://www.scottbuckley.com.au/library/wp-content/uploads/2020/03/sb_ephemera.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2020/06/sb_hiraeth.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2019/09/sb_neon.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2020/04/sb_signaltonoise.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2019/08/sb_sleep.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2020/05/sb_solace.mp3
https://www.scottbuckley.com.au/library/wp-content/uploads/2019/12/sb_undertow.mp3
'

sudo gem install ruby2d

