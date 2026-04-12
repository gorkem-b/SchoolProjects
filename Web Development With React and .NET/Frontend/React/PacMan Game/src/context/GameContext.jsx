import { createContext, useContext, useMemo, useReducer } from 'react'
import { createInitialState, gameReducer } from './GameReducer'
import { useGameLoop } from '../hooks/useGameLoop'
import { useKeyboard } from '../hooks/useKeyboard'

const GameContext = createContext(null)

export function GameProvider({ children }) {
  const [state, dispatch] = useReducer(gameReducer, undefined, createInitialState)

  useKeyboard(state.gameStatus, dispatch)
  useGameLoop(state, dispatch)

  const value = useMemo(() => ({ state, dispatch }), [state])

  return <GameContext.Provider value={value}>{children}</GameContext.Provider>
}

export function useGame() {
  const context = useContext(GameContext)

  if (!context) {
    throw new Error('useGame must be used within GameProvider')
  }

  return context
}
