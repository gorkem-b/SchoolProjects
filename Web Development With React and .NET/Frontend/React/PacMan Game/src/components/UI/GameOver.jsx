import styles from './UI.module.css'

export function GameOver({ isWin, score, onRestart }) {
  return (
    <div className={styles.overlay}>
      <div className={styles.panel}>
        <h2>{isWin ? 'Level Cleared' : 'You Were Caught'}</h2>
        <p>Score: {score}</p>
        <button type="button" onClick={onRestart} className={styles.actionButton}>
          Play Again
        </button>
      </div>
    </div>
  )
}
