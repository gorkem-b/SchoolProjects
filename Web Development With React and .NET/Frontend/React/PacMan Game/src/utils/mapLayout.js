import { TILE } from './constants'

export const MAP_LAYOUT = [
  [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
  [1, 0, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 3, 1],
  [1, 2, 1, 1, 2, 2, 1, 2, 1, 1, 1, 1, 2, 2, 1],
  [1, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 1, 2, 2, 1],
  [1, 2, 2, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 1],
  [1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 1],
  [1, 2, 1, 1, 0, 1, 2, 0, 2, 1, 0, 1, 1, 2, 1],
  [1, 2, 2, 2, 0, 2, 2, 0, 2, 2, 0, 2, 2, 2, 1],
  [1, 2, 1, 1, 0, 1, 2, 0, 2, 1, 0, 1, 1, 2, 1],
  [1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 1],
  [1, 2, 2, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 1],
  [1, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 1, 2, 2, 1],
  [1, 2, 1, 1, 2, 2, 1, 2, 1, 1, 1, 1, 2, 2, 1],
  [1, 3, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 0, 1],
  [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
]

export const BOARD_HEIGHT = MAP_LAYOUT.length
export const BOARD_WIDTH = MAP_LAYOUT[0].length

export function createInitialMap() {
  return MAP_LAYOUT.map((row) => row.slice())
}

export function countPellets(map) {
  return map.reduce((total, row) => {
    return (
      total +
      row.filter((tile) => tile === TILE.PELLET || tile === TILE.POWER_PELLET).length
    )
  }, 0)
}
