import { Pause, Play, SkipBack, SkipForward } from 'lucide-react'
import { usePlayer } from '../../context/PlayerContext'

const CONTROL_SIZES = {
  mini: { gap: 'gap-2.5', sidePad: 'p-2', mainPad: 'p-2.5', icon: 14, mainIcon: 17 },
  compact: { gap: 'gap-3', sidePad: 'p-2.5', mainPad: 'p-3', icon: 15, mainIcon: 18 },
  normal: { gap: 'gap-3.5', sidePad: 'p-3', mainPad: 'p-3.5', icon: 16, mainIcon: 19 },
}

function Controls({ density }) {
  const { isPlaying, togglePlay, nextSong, prevSong } = usePlayer()
  const size = CONTROL_SIZES[density]

  return (
    <div className={`flex items-center justify-center ${size.gap}`}>
      <button
        type="button"
        onClick={prevSong}
        className={`rounded-full bg-zinc-800 text-zinc-200 transition hover:bg-zinc-700 ${size.sidePad}`}
        aria-label="Previous song"
      >
        <SkipBack size={size.icon} />
      </button>

      <button
        type="button"
        onClick={togglePlay}
        className={`rounded-full bg-spotify-green text-black transition hover:brightness-110 ${size.mainPad}`}
        aria-label={isPlaying ? 'Pause' : 'Play'}
      >
        {/* Center icon glyphs visually for both states. */}
        {isPlaying ? (
          <Pause size={size.mainIcon} />
        ) : (
          <Play size={size.mainIcon} className="translate-x-px" />
        )}
      </button>

      <button
        type="button"
        onClick={nextSong}
        className={`rounded-full bg-zinc-800 text-zinc-200 transition hover:bg-zinc-700 ${size.sidePad}`}
        aria-label="Next song"
      >
        <SkipForward size={size.icon} />
      </button>
    </div>
  )
}

export default Controls
