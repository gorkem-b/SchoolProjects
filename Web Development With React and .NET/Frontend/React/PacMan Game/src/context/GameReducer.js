import {
  CARDINAL_DIRECTIONS,
  DIRECTION,
  GAME_STATUS,
  GHOST_STARTS,
  PACMAN_START,
  TILE,
} from '../utils/constants'
import { countPellets, createInitialMap } from '../utils/mapLayout'
import { getNextPosition, isSamePosition, reverseDirection, isWall } from '../utils/helpers'

export const ACTIONS = {
  START_GAME: 'START_GAME',
  SET_NEXT_DIRECTION: 'SET_NEXT_DIRECTION',
  APPLY_TICK: 'APPLY_TICK',
  RESET_GAME: 'RESET_GAME',
}

export function createInitialState() {
  return {
    map: createInitialMap(),
    pacmanPosition: { ...PACMAN_START },
    pacmanDirection: DIRECTION.NONE,
    nextDirection: DIRECTION.NONE,
    ghosts: GHOST_STARTS.map((ghost, index) => ({
      ...ghost,
      direction: index % 2 === 0 ? DIRECTION.LEFT : DIRECTION.RIGHT,
    })),
    score: 0,
    gameStatus: GAME_STATUS.START,
    pelletsLeft: countPellets(createInitialMap()),
  }
}

function canMoveTo(map, position) {
  return !isWall(map, position)
}

function resolvePacmanDirection(state) {
  // Apply buffered input first so the player can pre-turn at intersections.
  const candidatePosition = getNextPosition(state.pacmanPosition, state.nextDirection)

  if (
    state.nextDirection !== DIRECTION.NONE &&
    canMoveTo(state.map, candidatePosition)
  ) {
    return state.nextDirection
  }

  const currentPosition = getNextPosition(state.pacmanPosition, state.pacmanDirection)

  if (
    state.pacmanDirection !== DIRECTION.NONE &&
    canMoveTo(state.map, currentPosition)
  ) {
    return state.pacmanDirection
  }

  return DIRECTION.NONE
}

function getGhostDirections(map, ghost) {
  return CARDINAL_DIRECTIONS.filter((direction) => {
    const position = getNextPosition(ghost, direction)
    return canMoveTo(map, position)
  })
}

function moveGhosts(map, ghosts) {
  return ghosts.map((ghost) => {
    const options = getGhostDirections(map, ghost)

    if (options.length === 0) {
      return ghost
    }

    // Prefer not to reverse direction every frame to keep movement natural.
    const opposite = reverseDirection(ghost.direction)
    const preferred = options.filter((direction) => direction !== opposite)
    const choices = preferred.length > 0 ? preferred : options

    const randomIndex = Math.floor(Math.random() * choices.length)
    const direction = choices[randomIndex]
    const nextPosition = getNextPosition(ghost, direction)

    return {
      ...ghost,
      ...nextPosition,
      direction,
    }
  })
}

export function gameReducer(state, action) {
  if (action.type === ACTIONS.START_GAME) {
    return {
      ...state,
      gameStatus: GAME_STATUS.PLAYING,
    }
  }

  if (action.type === ACTIONS.SET_NEXT_DIRECTION) {
    return {
      ...state,
      nextDirection: action.payload,
    }
  }

  if (action.type === ACTIONS.RESET_GAME) {
    return createInitialState()
  }

  if (action.type === ACTIONS.APPLY_TICK) {
    if (state.gameStatus !== GAME_STATUS.PLAYING) {
      return state
    }

    // 1) Move pacman using buffered direction + current fallback.
    const nextDirection = resolvePacmanDirection(state)
    const nextPacmanPosition =
      nextDirection === DIRECTION.NONE
        ? state.pacmanPosition
        : getNextPosition(state.pacmanPosition, nextDirection)

    // 2) Consume pellet/power pellet on the destination tile.
    const nextMap = state.map.map((row) => row.slice())
    let scoreDelta = 0

    const tile = nextMap[nextPacmanPosition.y][nextPacmanPosition.x]

    if (tile === TILE.PELLET) {
      scoreDelta = 10
      nextMap[nextPacmanPosition.y][nextPacmanPosition.x] = TILE.EMPTY
    }

    if (tile === TILE.POWER_PELLET) {
      scoreDelta = 50
      nextMap[nextPacmanPosition.y][nextPacmanPosition.x] = TILE.EMPTY
    }

    // 3) Move ghosts and resolve end-game states.
    const nextGhosts = moveGhosts(nextMap, state.ghosts)
    const ghostCollision = nextGhosts.some((ghost) =>
      isSamePosition(ghost, nextPacmanPosition),
    )

    const pelletsLeft = countPellets(nextMap)

    if (ghostCollision) {
      return {
        ...state,
        pacmanPosition: nextPacmanPosition,
        pacmanDirection: nextDirection,
        map: nextMap,
        score: state.score + scoreDelta,
        ghosts: nextGhosts,
        gameStatus: GAME_STATUS.GAME_OVER,
        pelletsLeft,
      }
    }

    const won = pelletsLeft === 0

    return {
      ...state,
      pacmanPosition: nextPacmanPosition,
      pacmanDirection: nextDirection,
      map: nextMap,
      score: state.score + scoreDelta,
      ghosts: nextGhosts,
      gameStatus: won ? GAME_STATUS.WIN : GAME_STATUS.PLAYING,
      pelletsLeft,
    }
  }

  return state
}
