import React from 'react';
import './Card.css';

const Card = ({ card, isSelected, isVisible, isClickable = true, onClick }) => {
  const getSuitSymbol = (suit) => {
    switch(suit) {
      case 'Hearts': return '♥';
      case 'Diamonds': return '♦';
      case 'Clubs': return '♣';
      case 'Spades': return '♠';
      default: return '';
    }
  };

  const getSuitColor = (suit) => {
    return (suit === 'Hearts' || suit === 'Diamonds') ? 'red' : 'black';
  };

  return (
    <div 
      className={`card ${isVisible ? 'visible' : 'hidden'} ${isSelected ? 'selected' : ''} ${card.isMatching ? 'matching' : ''} ${isClickable ? '' : 'disabled'}`}
      onClick={isClickable ? onClick : undefined}
    >
      {isVisible ? (
        <div className="card-front">
          <div className={`card-rank ${getSuitColor(card.suit)}`}>
            {card.rank}
          </div>
          <div className={`card-suit ${getSuitColor(card.suit)}`}>
            {getSuitSymbol(card.suit)}
          </div>
          <div className={`card-rank bottom ${getSuitColor(card.suit)}`}>
            {card.rank}
          </div>
        </div>
      ) : (
        <div className="card-back">
          <div className="card-pattern">♠♣♥♦</div>
        </div>
      )}
    </div>
  );
};

export default Card;