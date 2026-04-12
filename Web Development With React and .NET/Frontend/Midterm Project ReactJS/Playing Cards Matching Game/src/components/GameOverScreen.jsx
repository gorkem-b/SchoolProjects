import React from 'react';
import './GameOverScreen.css';

const GameOverScreen = ({ moves, score, onRestart }) => {
  return (
    <div className="game-over-screen">
      <div className="game-over-content">
        <h2>Congratulations!</h2>
        <p>You've completed the game!</p>
        <p>Total moves: <span className="move-count">{moves}</span></p>
        <p>Final score: <span className="score-count">{score}</span></p>
        <button className="restart-button" onClick={onRestart}>
          Play Again
        </button>
      </div>
    </div>
  );
};

export default GameOverScreen;