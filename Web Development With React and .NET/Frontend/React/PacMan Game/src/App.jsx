import { GameBoard } from './components/GameBoard'
import { GameProvider } from './context/GameContext'
import './App.css'

function App() {
  return (
    <GameProvider>
      <div className="app-root">
        <GameBoard />
      </div>
    </GameProvider>
  )
}

export default App
