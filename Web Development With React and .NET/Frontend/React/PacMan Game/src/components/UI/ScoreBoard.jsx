import { GAME_STATUS } from '../../utils/constants'
import styles from './UI.module.css'

const STATUS_LABEL = {
  [GAME_STATUS.START]: 'Ready',
  [GAME_STATUS.PLAYING]: 'Playing',
  [GAME_STATUS.GAME_OVER]: 'Game Over',
  [GAME_STATUS.WIN]: 'Victory',
}

export function ScoreBoard({ score, gameStatus, pelletsLeft }) {
  return (
    <header className={styles.scoreBoard}>
      <div className={styles.brandBox}>
        <img className={styles.brandLogo} src="/logo.svg" alt="Pacman Arena logo" />
        <span className={styles.brandName}>Pacman Arena</span>
      </div>
      <p>Score: {score}</p>
      <p>Status: {STATUS_LABEL[gameStatus]}</p>
      <p>Pellets Left: {pelletsLeft}</p>
    </header>
  )
}
