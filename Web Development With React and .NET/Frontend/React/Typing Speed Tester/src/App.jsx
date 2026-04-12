import Header from './components/Header/Header'
import ResetButton from './components/ResetButton/ResetButton'
import Stats from './components/Stats/Stats'
import Timer from './components/Timer/Timer'
import TypingArea from './components/TypingArea/TypingArea'
import { useTypingLogic } from './hooks/useTypingLogic'
import './App.css'

function App() {
  const { text, userInput, timeLeft, isFinished, stats, handleInput, reset } =
    useTypingLogic()

  return (
    <main className="app">
      <div className="panel">
        <div className="top">
          <Header />
          <Timer timeLeft={timeLeft} isFinished={isFinished} />
        </div>

        <TypingArea
          text={text}
          userInput={userInput}
          isFinished={isFinished}
          onInputChange={handleInput}
        />

        <Stats stats={stats} />

        <div className="actions">
          <ResetButton onReset={reset} />
        </div>
      </div>
    </main>
  )
}

export default App
