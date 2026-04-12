import { useGame } from '../../context/GameContext'
import { ACTIONS } from '../../context/GameReducer'
import { CELL_SIZE, GAME_STATUS } from '../../utils/constants'
import { BOARD_HEIGHT, BOARD_WIDTH } from '../../utils/mapLayout'
import { Cell } from '../Grid/Cell'
import { Ghost } from '../Entity/Ghost'
import { Pacman } from '../Entity/Pacman'
import { GameOver } from '../UI/GameOver'
import { ScoreBoard } from '../UI/ScoreBoard'
import { StartScreen } from '../UI/StartScreen'
import styles from './GameBoard.module.css'

export function GameBoard() {
  const { state, dispatch } = useGame()

  const boardStyle = {
    width: BOARD_WIDTH * CELL_SIZE,
    height: BOARD_HEIGHT * CELL_SIZE,
    gridTemplateColumns: `repeat(${BOARD_WIDTH}, ${CELL_SIZE}px)`,
  }

  return (
    <main className={styles.page}>
      <section className={styles.shell}>
        <ScoreBoard
          score={state.score}
          gameStatus={state.gameStatus}
          pelletsLeft={state.pelletsLeft}
        />

        <div className={styles.boardWrap}>
          <div className={styles.board} style={boardStyle}>
            {state.map.flatMap((row, y) =>
              row.map((tile, x) => <Cell key={`${x}-${y}`} tile={tile} />),
            )}
          </div>

          <div className={styles.entities} style={boardStyle}>
            <Pacman position={state.pacmanPosition} direction={state.pacmanDirection} />
            {state.ghosts.map((ghost) => (
              <Ghost key={ghost.id} ghost={ghost} />
            ))}
          </div>

          {state.gameStatus === GAME_STATUS.START && (
            <StartScreen onStart={() => dispatch({ type: ACTIONS.START_GAME })} />
          )}

          {(state.gameStatus === GAME_STATUS.GAME_OVER ||
            state.gameStatus === GAME_STATUS.WIN) && (
            <GameOver
              isWin={state.gameStatus === GAME_STATUS.WIN}
              score={state.score}
              onRestart={() => dispatch({ type: ACTIONS.RESET_GAME })}
            />
          )}
        </div>
      </section>
    </main>
  )
}
