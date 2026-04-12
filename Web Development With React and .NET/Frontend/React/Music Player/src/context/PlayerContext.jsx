import { createContext, useCallback, useContext, useMemo, useState } from 'react'
import songs from '../data/songs.json'
import { useAudio } from '../hooks/useAudio'

const PlayerContext = createContext(null)

export function PlayerProvider({ children }) {
  // Core playback state is centralized here to avoid prop drilling across UI layers.
  const [currentSongIndex, setCurrentSongIndex] = useState(0)
  const [isPlaying, setIsPlaying] = useState(false)
  const [volume, setVolume] = useState(0.7)
  const [currentTime, setCurrentTime] = useState(0)
  const [duration, setDuration] = useState(0)

  const currentSong = songs[currentSongIndex]

  const nextSong = useCallback(() => {
    setCurrentTime(0)
    setDuration(0)
    setCurrentSongIndex((prev) => (prev + 1) % songs.length)
  }, [])

  const prevSong = useCallback(() => {
    setCurrentTime(0)
    setDuration(0)
    setCurrentSongIndex((prev) => (prev - 1 + songs.length) % songs.length)
  }, [])

  const playSong = useCallback((songId) => {
    const nextIndex = songs.findIndex((song) => song.id === songId)
    if (nextIndex === -1) {
      return
    }

    setCurrentSongIndex(nextIndex)
    setCurrentTime(0)
    setDuration(0)
    setIsPlaying(true)
  }, [])

  const togglePlay = useCallback(() => {
    setIsPlaying((prev) => !prev)
  }, [])

  const seek = useCallback((time) => {
    setCurrentTime(time)
  }, [])

  const updateVolume = useCallback((value) => {
    setVolume(value)
  }, [])

  // If a track fails to load/play, skip forward to keep the player responsive.
  const handleAudioError = useCallback(() => {
    nextSong()
    setIsPlaying(true)
  }, [nextSong])

  const audioRef = useAudio({
    currentSong,
    isPlaying,
    volume,
    onTimeUpdate: setCurrentTime,
    onDurationChange: setDuration,
    onEnded: nextSong,
    onError: handleAudioError,
  })

  const value = useMemo(
    () => ({
      songs,
      currentSong,
      currentSongIndex,
      isPlaying,
      volume,
      currentTime,
      duration,
      audioRef,
      togglePlay,
      nextSong,
      prevSong,
      playSong,
      seek,
      setVolume: updateVolume,
    }),
    [
      currentSong,
      currentSongIndex,
      isPlaying,
      volume,
      currentTime,
      duration,
      audioRef,
      togglePlay,
      nextSong,
      prevSong,
      playSong,
      seek,
      updateVolume,
    ],
  )

  return <PlayerContext.Provider value={value}>{children}</PlayerContext.Provider>
}

export function usePlayer() {
  const context = useContext(PlayerContext)

  if (!context) {
    throw new Error('usePlayer must be used inside PlayerProvider')
  }

  return context
}
