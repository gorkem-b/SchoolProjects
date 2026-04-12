import React from 'react';
import Card from './Card';
import './CardGroup.css';

const CardGroup = ({ group, groupIndex, onCardClick, selectedCards }) => {
  // Bu grupta kullanıcıya gösterilecek üst kartı bul.
  let topCardIndex = -1;
  for (let i = group.length - 1; i >= 0; i--) {
    if (!group[i].isMatched) {
      topCardIndex = i;
      break;
    }
  }

  // Seçili efektini göstermek için grup bazlı seçim kontrolü.
  const isSelected = selectedCards.some(
    selected => selected.groupIndex === groupIndex
  );

  const topCard = topCardIndex >= 0 ? group[topCardIndex] : null;

  return (
    <div className="card-group">
      {topCard ? (
        <div className="card-wrapper">
          <Card
            card={topCard}
            isVisible={topCard.isFlipped}
            isSelected={isSelected}
            isClickable={!topCard.isFlipped}
            onClick={() => onCardClick(groupIndex)}
          />
        </div>
      ) : (
        <div className="card-slot" aria-hidden="true" />
      )}
    </div>
  );
};

export default CardGroup;