import { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { getRandomParagraph } from '../utils/paragraphs'
import {
  calculateAccuracy,
  calculateCPM,
  calculateWPM,
  countErrors,
  countWords,
} from '../utils/calculators'

const TEST_DURATION = 60

export function useTypingLogic() {
  const [text, setText] = useState(() => getRandomParagraph())
  const [userInput, setUserInput] = useState('')
  const [timeLeft, setTimeLeft] = useState(TEST_DURATION)
  const [isTyping, setIsTyping] = useState(false)
  const [isFinished, setIsFinished] = useState(false)
  const [elapsedSeconds, setElapsedSeconds] = useState(0)

  const startedAtRef = useRef(null)

  useEffect(() => {
    if (!isTyping || isFinished || startedAtRef.current === null) return undefined

    // Update elapsed time with sub-second precision while typing is active.
    const timer = window.setInterval(() => {
      const elapsed = (Date.now() - startedAtRef.current) / 1000
      const remain = Math.max(0, TEST_DURATION - elapsed)

      setElapsedSeconds(elapsed)
      setTimeLeft(Math.ceil(remain))

      if (remain <= 0) {
        setIsTyping(false)
        setIsFinished(true)
      }
    }, 100)

    return () => window.clearInterval(timer)
  }, [isTyping, isFinished])

  const totalChars = userInput.length
  const errors = useMemo(() => countErrors(userInput, text), [userInput, text])
  const correctChars = Math.max(0, totalChars - errors)
  const words = useMemo(() => countWords(userInput), [userInput])

  // Keep all public test metrics in one memoized object.
  const stats = useMemo(
    () => ({
      wpm: calculateWPM(correctChars, elapsedSeconds),
      cpm: calculateCPM(correctChars, elapsedSeconds),
      errors,
      accuracy: calculateAccuracy(errors, totalChars),
      words,
      totalChars,
    }),
    [correctChars, elapsedSeconds, errors, totalChars, words],
  )

  const handleInput = useCallback(
    (value) => {
      if (isFinished) return

      // The timer starts on the first meaningful user input.
      if (!isTyping) {
        startedAtRef.current = Date.now()
        setIsTyping(true)
      }

      setUserInput(value)
    },
    [isFinished, isTyping],
  )

  const reset = useCallback(() => {
    // Reset all state and rotate to a different paragraph.
    startedAtRef.current = null
    setText((prev) => getRandomParagraph(prev))
    setUserInput('')
    setTimeLeft(TEST_DURATION)
    setIsTyping(false)
    setIsFinished(false)
    setElapsedSeconds(0)
  }, [])

  return {
    text,
    userInput,
    timeLeft,
    isTyping,
    isFinished,
    stats,
    handleInput,
    reset,
  }
}
