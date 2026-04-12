import { ACTIONS, createInitialState, gameReducer } from './GameReducer'
import { GAME_STATUS } from '../utils/constants'

describe('gameReducer movement', () => {
  test('moves pacman and increases score when pellet is eaten', () => {
    let state = createInitialState()
    state = gameReducer(state, { type: ACTIONS.START_GAME })
    state = gameReducer(state, {
      type: ACTIONS.SET_NEXT_DIRECTION,
      payload: 'RIGHT',
    })

    const nextState = gameReducer(state, { type: ACTIONS.APPLY_TICK })

    expect(nextState.pacmanPosition).toEqual({ x: 2, y: 1 })
    expect(nextState.score).toBe(10)
  })

  test('does not move into wall', () => {
    let state = createInitialState()
    state = gameReducer(state, { type: ACTIONS.START_GAME })
    state = gameReducer(state, {
      type: ACTIONS.SET_NEXT_DIRECTION,
      payload: 'UP',
    })

    const nextState = gameReducer(state, { type: ACTIONS.APPLY_TICK })

    expect(nextState.pacmanPosition).toEqual({ x: 1, y: 1 })
    expect(nextState.gameStatus).toBe(GAME_STATUS.PLAYING)
  })
})
