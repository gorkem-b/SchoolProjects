import { BOARD_HEIGHT, BOARD_WIDTH } from './mapLayout'
import { DIRECTION, TILE } from './constants'

export function cloneMap(map) {
  return map.map((row) => row.slice())
}

export function isInsideBoard(position) {
  return (
    position.x >= 0 &&
    position.x < BOARD_WIDTH &&
    position.y >= 0 &&
    position.y < BOARD_HEIGHT
  )
}

export function getNextPosition(position, direction) {
  if (direction === DIRECTION.UP) {
    return { x: position.x, y: position.y - 1 }
  }

  if (direction === DIRECTION.DOWN) {
    return { x: position.x, y: position.y + 1 }
  }

  if (direction === DIRECTION.LEFT) {
    return { x: position.x - 1, y: position.y }
  }

  if (direction === DIRECTION.RIGHT) {
    return { x: position.x + 1, y: position.y }
  }

  return position
}

export function getTile(map, position) {
  // Treat out-of-bounds as walls to keep movement checks simple.
  if (!isInsideBoard(position)) {
    return TILE.WALL
  }

  return map[position.y][position.x]
}

export function isWall(map, position) {
  return getTile(map, position) === TILE.WALL
}

export function isSamePosition(first, second) {
  return first.x === second.x && first.y === second.y
}

export function reverseDirection(direction) {
  if (direction === DIRECTION.UP) {
    return DIRECTION.DOWN
  }

  if (direction === DIRECTION.DOWN) {
    return DIRECTION.UP
  }

  if (direction === DIRECTION.LEFT) {
    return DIRECTION.RIGHT
  }

  if (direction === DIRECTION.RIGHT) {
    return DIRECTION.LEFT
  }

  return DIRECTION.NONE
}
