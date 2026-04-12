// Standard typing formula: one word equals five characters.
export function calculateWPM(correctChars, elapsedSeconds) {
  if (!correctChars) return 0
  const minutes = Math.max(elapsedSeconds, 1) / 60
  return Math.round(correctChars / 5 / minutes)
}

// CPM counts only correct characters per minute.
export function calculateCPM(correctChars, elapsedSeconds) {
  if (!correctChars) return 0
  const minutes = Math.max(elapsedSeconds, 1) / 60
  return Math.round(correctChars / minutes)
}

// Accuracy is bounded at zero to avoid negative values in edge cases.
export function calculateAccuracy(errors, totalChars) {
  if (!totalChars) return 100
  return Math.max(0, Math.round(((totalChars - errors) / totalChars) * 100))
}

// Compare typed input with target text character by character.
export function countErrors(input, target) {
  let errors = 0
  for (let i = 0; i < input.length; i += 1) {
    if (input[i] !== target[i]) errors += 1
  }
  return errors
}

// Split by whitespace to estimate typed words.
export function countWords(input) {
  if (!input.trim()) return 0
  return input.trim().split(/\s+/).length
}
