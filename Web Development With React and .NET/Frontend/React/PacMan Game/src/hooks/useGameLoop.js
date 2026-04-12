import { useEffect } from 'react'
import { ACTIONS } from '../context/GameReducer'
import { GAME_SPEED, GAME_STATUS } from '../utils/constants'

export function useGameLoop(state, dispatch) {
  useEffect(() => {
    // Stop ticking on overlays (start, game over, win) to freeze the board state.
    if (state.gameStatus !== GAME_STATUS.PLAYING) {
      return undefined
    }

    const intervalId = window.setInterval(() => {
      dispatch({ type: ACTIONS.APPLY_TICK })
    }, GAME_SPEED)

    return () => {
      window.clearInterval(intervalId)
    }
  }, [dispatch, state.gameStatus])
}
