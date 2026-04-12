import React from 'react';
import './Header.css';

const Header = ({ moves, score, onRestart }) => {
  return (
    <div className="header">
      <div className="stats">
        <div className="move-counter">
          Moves: <span className="move-count">{moves}</span>
        </div>
        <div className="score-counter">
          Score: <span className="score-count">{score}</span>
        </div>
      </div>
      <button className="restart-button" onClick={onRestart}>
        Restart Game
      </button>
    </div>
  );
};

export default Header;