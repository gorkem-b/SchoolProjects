import { TILE } from '../../utils/constants'
import styles from './Cell.module.css'
import { Wall } from './Wall'

export function Cell({ tile }) {
  if (tile === TILE.WALL) {
    return <Wall />
  }

  return (
    <div className={styles.cell}>
      {tile === TILE.PELLET && <span className={styles.pellet} aria-hidden="true" />}
      {tile === TILE.POWER_PELLET && (
        <span className={styles.powerPellet} aria-hidden="true" />
      )}
    </div>
  )
}
