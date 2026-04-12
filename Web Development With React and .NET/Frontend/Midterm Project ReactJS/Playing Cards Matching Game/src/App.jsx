import './App.css';
import Header from './components/Header';
import GameBoard from './components/GameBoard';
import GameOverScreen from './components/GameOverScreen';
import { useGame } from './hooks/useGame';

function App() {
  const {
    cardGroups,
    activeSelection,
    moves,
    score,
    gameOver,
    handleCardClick,
    restartGame
  } = useGame();

  return (
    <div className="app">
      <Header moves={moves} score={score} onRestart={restartGame} />
      <GameBoard 
        cardGroups={cardGroups} 
        onCardClick={handleCardClick}
        selectedCards={activeSelection}
      />
      {gameOver && (
        <GameOverScreen moves={moves} score={score} onRestart={restartGame} />
      )}
    </div>
  );
}

export default App
