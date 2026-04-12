import { useMemo } from 'react'
import styles from './TypingArea.module.css'

function TypingArea({ text, userInput, isFinished, onInputChange }) {
  const chars = useMemo(() => text.split(''), [text])

  return (
    <section className={styles.wrapper}>
      <p className={styles.text} aria-label="Target text">
        {chars.map((char, index) => {
          // Mark each rendered character based on current user input state.
          let className = styles.pending
          if (index < userInput.length) {
            className = userInput[index] === char ? styles.correct : styles.incorrect
          }

          return (
            <span key={`${char}-${index}`} className={className}>
              {char}
            </span>
          )
        })}
      </p>

      <label className={styles.label} htmlFor="typing-input">
        Start typing
      </label>
      <textarea
        id="typing-input"
        className={styles.input}
        value={userInput}
        disabled={isFinished}
        onChange={(event) => onInputChange(event.target.value)}
        placeholder="Type here to start the test..."
        autoComplete="off"
        autoCorrect="off"
        autoCapitalize="off"
        spellCheck={false}
      />
    </section>
  )
}

export default TypingArea
