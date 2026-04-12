import styles from './UI.module.css'

export function StartScreen({ onStart }) {
  return (
    <div className={styles.overlay}>
      <div className={styles.panel}>
        <img className={styles.panelLogo} src="/logo.svg" alt="Pacman Arena logo" />
        <h1>Pacman Arena</h1>
        <p>Use the arrow keys to move. Press Enter to start.</p>
        <button type="button" onClick={onStart} className={styles.actionButton}>
          Start Game
        </button>
      </div>
    </div>
  )
}
