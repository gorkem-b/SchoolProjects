import { Volume2, VolumeX } from 'lucide-react'
import { usePlayer } from '../../context/PlayerContext'

const VOLUME_STYLES = {
  mini: { gap: 'gap-2', icon: 14, slider: 'h-1.5' },
  compact: { gap: 'gap-2.5', icon: 15, slider: 'h-2' },
  normal: { gap: 'gap-3', icon: 16, slider: 'h-2.5' },
}

function Volume({ density }) {
  const { volume, setVolume, audioRef } = usePlayer()
  const styles = VOLUME_STYLES[density]

  const handleVolume = (event) => {
    // Keep React state and native audio volume fully in sync.
    const nextVolume = Number(event.target.value)
    setVolume(nextVolume)

    if (audioRef.current) {
      audioRef.current.volume = nextVolume
    }
  }

  return (
    <div className={`flex items-center ${styles.gap}`}>
      {volume === 0 ? (
        <VolumeX size={styles.icon} className="text-zinc-400" />
      ) : (
        <Volume2 size={styles.icon} className="text-zinc-300" />
      )}
      <input
        type="range"
        min={0}
        max={1}
        step={0.01}
        value={volume}
        onChange={handleVolume}
        className={`player-range w-full cursor-pointer ${styles.slider}`}
        aria-label="Volume level"
      />
    </div>
  )
}

export default Volume
