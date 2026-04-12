export function formatTime(seconds) {
  // Guard against invalid values so UI labels stay predictable.
  if (!Number.isFinite(seconds) || seconds < 0) {
    return '0:00'
  }

  const totalSeconds = Math.floor(seconds)
  const mins = Math.floor(totalSeconds / 60)
  const secs = totalSeconds % 60

  return `${mins}:${secs.toString().padStart(2, '0')}`
}
