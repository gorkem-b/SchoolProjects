import React from 'react';
import CardGroup from './CardGroup';
import './GameBoard.css';

const GameBoard = ({ cardGroups, onCardClick, selectedCards }) => {
  // Tahta düzeni sabit: 4 + 5 + 4 grup.
  const firstRow = cardGroups.slice(0, 4);
  const secondRow = cardGroups.slice(4, 9);
  const thirdRow = cardGroups.slice(9, 13);

  return (
    <div className="game-board">
      <div className="game-board-row row-first">
        {firstRow.map((group, groupIndex) => (
          <CardGroup
            key={`group-${groupIndex}`}
            group={group}
            groupIndex={groupIndex}
            onCardClick={onCardClick}
            selectedCards={selectedCards}
          />
        ))}
      </div>
      <div className="game-board-row row-second">
        {secondRow.map((group, groupIndex) => (
          <CardGroup
            key={`group-${groupIndex + 4}`}
            group={group}
            groupIndex={groupIndex + 4}
            onCardClick={onCardClick}
            selectedCards={selectedCards}
          />
        ))}
      </div>
      <div className="game-board-row row-third">
        {thirdRow.map((group, groupIndex) => (
          <CardGroup
            key={`group-${groupIndex + 9}`}
            group={group}
            groupIndex={groupIndex + 9}
            onCardClick={onCardClick}
            selectedCards={selectedCards}
          />
        ))}
      </div>
    </div>
  );
};

export default GameBoard;
