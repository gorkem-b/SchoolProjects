import { CELL_SIZE } from '../../utils/constants'
import styles from './Entity.module.css'

export function Ghost({ ghost }) {
  return (
    <div
      className={styles.ghost}
      style={{
        width: CELL_SIZE,
        height: CELL_SIZE,
        top: ghost.y * CELL_SIZE,
        left: ghost.x * CELL_SIZE,
        '--ghost-color': ghost.color,
      }}
      aria-label={`${ghost.id} ghost`}
    >
      <span className={styles.ghostEye} />
      <span className={styles.ghostEye} />
    </div>
  )
}
