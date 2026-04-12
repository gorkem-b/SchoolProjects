import { memo } from 'react'
import { usePlayer } from '../../context/PlayerContext'
import { formatTime } from '../../utils/formatTime'

const PROGRESS_STYLES = {
  mini: { space: 'space-y-1', slider: 'h-1.5', text: 'text-[10px]' },
  compact: { space: 'space-y-1.5', slider: 'h-2', text: 'text-[11px]' },
  normal: { space: 'space-y-2', slider: 'h-2.5', text: 'text-xs' },
}

function Progress({ density }) {
  const { currentTime, duration, seek, audioRef } = usePlayer()
  const styles = PROGRESS_STYLES[density]

  const handleSeek = (event) => {
    // Mirror slider movement directly to both state and the active audio element.
    const nextTime = Number(event.target.value)
    seek(nextTime)

    if (audioRef.current) {
      audioRef.current.currentTime = nextTime
    }
  }

  return (
    <div className={styles.space}>
      <input
        type="range"
        min={0}
        max={duration || 0}
        value={currentTime}
        onChange={handleSeek}
        className={`player-range w-full cursor-pointer ${styles.slider}`}
        aria-label="Track progress"
      />
      <div className={`flex justify-between text-zinc-400 ${styles.text}`}>
        <span>{formatTime(currentTime)}</span>
        <span>{formatTime(duration)}</span>
      </div>
    </div>
  )
}

export default memo(Progress)
