import { CELL_SIZE, DIRECTION } from '../../utils/constants'
import styles from './Entity.module.css'

const ROTATION = {
  [DIRECTION.RIGHT]: 'rotate(0deg)',
  [DIRECTION.DOWN]: 'rotate(90deg)',
  [DIRECTION.LEFT]: 'rotate(180deg)',
  [DIRECTION.UP]: 'rotate(270deg)',
  [DIRECTION.NONE]: 'rotate(0deg)',
}

export function Pacman({ position, direction }) {
  return (
    <div
      className={styles.pacman}
      style={{
        width: CELL_SIZE,
        height: CELL_SIZE,
        top: position.y * CELL_SIZE,
        left: position.x * CELL_SIZE,
        transform: ROTATION[direction],
      }}
      aria-label="Pacman"
    />
  )
}
