import styles from './Timer.module.css'

function Timer({ timeLeft, isFinished }) {
  return (
    <div className={styles.timer}>
      <span>Time</span>
      <strong className={isFinished ? styles.finished : ''}>{timeLeft}s</strong>
    </div>
  )
}

export default Timer
