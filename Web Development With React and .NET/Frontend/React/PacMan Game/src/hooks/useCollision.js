import { useCallback } from 'react'
import { TILE } from '../utils/constants'
import { getTile, isSamePosition } from '../utils/helpers'

export function consumePellet(map, position) {
  const tile = map[position.y][position.x]

  if (tile === TILE.PELLET) {
    map[position.y][position.x] = TILE.EMPTY
    return 10
  }

  if (tile === TILE.POWER_PELLET) {
    map[position.y][position.x] = TILE.EMPTY
    return 50
  }

  return 0
}

export function useCollision(map, ghosts) {
  const isWallCollision = useCallback(
    (position) => getTile(map, position) === TILE.WALL,
    [map],
  )

  const hasGhostCollision = useCallback(
    (position) => ghosts.some((ghost) => isSamePosition(ghost, position)),
    [ghosts],
  )

  return {
    isWallCollision,
    hasGhostCollision,
  }
}
