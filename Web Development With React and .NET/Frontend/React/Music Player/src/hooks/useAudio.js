import { useEffect, useRef } from 'react'

export function useAudio({
  currentSong,
  isPlaying,
  volume,
  onTimeUpdate,
  onDurationChange,
  onEnded,
  onError,
}) {
  const audioRef = useRef(null)

  // Lazily instantiate a single audio element and keep it stable across renders.
  if (audioRef.current == null) {
    audioRef.current = new Audio(currentSong.src)
  }

  useEffect(() => {
    const audio = audioRef.current

    const handleTimeUpdate = () => {
      onTimeUpdate(audio.currentTime)
    }

    const handleMetadataLoaded = () => {
      onDurationChange(audio.duration || 0)
    }

    const handleEnded = () => {
      onEnded()
    }

    const handleError = () => {
      onError()
    }

    audio.addEventListener('timeupdate', handleTimeUpdate)
    audio.addEventListener('loadedmetadata', handleMetadataLoaded)
    audio.addEventListener('ended', handleEnded)
    audio.addEventListener('error', handleError)

    // Remove listeners during dependency changes to prevent duplicate subscriptions.
    return () => {
      audio.removeEventListener('timeupdate', handleTimeUpdate)
      audio.removeEventListener('loadedmetadata', handleMetadataLoaded)
      audio.removeEventListener('ended', handleEnded)
      audio.removeEventListener('error', handleError)
    }
  }, [onDurationChange, onEnded, onError, onTimeUpdate])

  useEffect(() => {
    const audio = audioRef.current

    // Swap sources only when the selected track changes.
    if (audio.src !== window.location.origin + currentSong.src) {
      audio.src = currentSong.src
      audio.load()
    }

    if (isPlaying) {
      const playPromise = audio.play()
      if (playPromise?.catch) {
        playPromise.catch(() => {
          onError()
        })
      }
    } else {
      audio.pause()
    }
  }, [currentSong, isPlaying, onError])

  useEffect(() => {
    audioRef.current.volume = volume
  }, [volume])

  return audioRef
}
