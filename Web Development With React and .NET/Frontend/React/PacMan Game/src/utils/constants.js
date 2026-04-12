export const CELL_SIZE = 28
export const GAME_SPEED = 180

export const TILE = {
  EMPTY: 0,
  WALL: 1,
  PELLET: 2,
  POWER_PELLET: 3,
}

export const DIRECTION = {
  UP: 'UP',
  DOWN: 'DOWN',
  LEFT: 'LEFT',
  RIGHT: 'RIGHT',
  NONE: 'NONE',
}

export const CARDINAL_DIRECTIONS = [
  DIRECTION.UP,
  DIRECTION.DOWN,
  DIRECTION.LEFT,
  DIRECTION.RIGHT,
]

export const GAME_STATUS = {
  START: 'START',
  PLAYING: 'PLAYING',
  GAME_OVER: 'GAME_OVER',
  WIN: 'WIN',
}

export const PACMAN_START = { x: 1, y: 1 }

export const GHOST_STARTS = [
  { id: 'blinky', x: 7, y: 6, color: '#ff5a5f' },
  { id: 'inky', x: 8, y: 7, color: '#3ad5ff' },
  { id: 'pinky', x: 7, y: 8, color: '#ff7ad9' },
]
