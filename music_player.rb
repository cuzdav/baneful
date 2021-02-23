require 'ruby2d'

# ruby2d does not have a way to discover when a song has finished playing
# So, this uses a list of files paired with the duration (in seconds) for
# how long that song is.  Then it plays one, and waits until it logically
# finished, and begins the next track.  If you pause a track, it will add
# that much pause time to the stop time, so the song can still complete.

# I wish there was a better way but so far I have not found one.

class MusicPlayer
  attr_reader :music_playlist

  def volume_up(pct)
    @volume += pct
    if @volume > 100
      @volume = 100
    end
    puts ("volume up: #{@volume}")
    @music.volume = @volume
  end

  def volume_down(pct)
    @volume -= pct
    if @volume < 0
      @volume = 0
    end
    puts ("volume down: #{@volume}")
    @music.volume = @volume
  end

  def pause()
    puts ("pause")
    @pause_time = Time.now
    @music.pause
  end

  def resume()
    if @pause_time != nil
      puts ("resume")
      paused_duration = Time.now - @pause_time
      puts("ADDING #{paused_duration} to stop time...")
      @stop_time += paused_duration
      @music.resume
      @pause_time = nil
    end
  end

  def next_track
    puts("NEXT TRACK")
    @music.stop
    @index += 1
    play_next_track
  end

  def exit()
    puts ("exit")
    @music.stop
  end

  def initialize(music_playlist)
    @music_playlist = music_playlist
    @volume = 60
    @index = 0

    #verify
    @music_playlist.each do |file, _|
      if not File.exists?("resource/" + file)
        raise "Music File Not Found #{file}"
      end
    end

    play_next_track()
  end

  def update()
    if @pause_time == nil and Time.now >= @stop_time
      next_track
    end
  end

  def play_next_track
    music_file, duration = @music_playlist[@index % @music_playlist.size]
    puts("Next song: #{music_file}")
    @music = Music.new("resource/" + music_file)
    @music.volume = @volume

    @stop_time = Time.now + duration
    @music.play
  end
end
