import { useEffect } from 'react'
import { ACTIONS } from '../context/GameReducer'
import { DIRECTION, GAME_STATUS } from '../utils/constants'

const KEY_TO_DIRECTION = {
  ArrowUp: DIRECTION.UP,
  ArrowDown: DIRECTION.DOWN,
  ArrowLeft: DIRECTION.LEFT,
  ArrowRight: DIRECTION.RIGHT,
}

export function useKeyboard(gameStatus, dispatch) {
  useEffect(() => {
    function onKeyDown(event) {
      const nextDirection = KEY_TO_DIRECTION[event.key]

      if (nextDirection) {
        // Prevent page scroll on arrow keys while playing.
        event.preventDefault()
        dispatch({ type: ACTIONS.SET_NEXT_DIRECTION, payload: nextDirection })
      }

      if (event.key === 'Enter' && gameStatus === GAME_STATUS.START) {
        dispatch({ type: ACTIONS.START_GAME })
      }

      if (
        event.key === 'Enter' &&
        (gameStatus === GAME_STATUS.GAME_OVER || gameStatus === GAME_STATUS.WIN)
      ) {
        dispatch({ type: ACTIONS.RESET_GAME })
      }
    }

    window.addEventListener('keydown', onKeyDown)
    return () => {
      window.removeEventListener('keydown', onKeyDown)
    }
  }, [dispatch, gameStatus])
}
