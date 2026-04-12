import { describe, it, expect } from 'vitest';
import {
  createDeck,
  shuffleDeck,
  groupDeck,
  initializeGame,
  hasAvailableMoves,
  stabilizeGroups
} from './deck';

function isLayoutSolvable(groups) {
  const topIndexes = groups.map((group) => group.length - 1);
  const memo = new Map();

  const solve = (indexes) => {
    const key = indexes.join(',');
    if (memo.has(key)) {
      return memo.get(key);
    }

    if (indexes.every((index) => index < 0)) {
      memo.set(key, true);
      return true;
    }

    const groupsByRank = new Map();
    for (let groupIndex = 0; groupIndex < indexes.length; groupIndex++) {
      const cardIndex = indexes[groupIndex];
      if (cardIndex < 0) {
        continue;
      }

      const rank = groups[groupIndex][cardIndex].rank;
      const rankGroups = groupsByRank.get(rank) || [];
      rankGroups.push(groupIndex);
      groupsByRank.set(rank, rankGroups);
    }

    for (const rankGroups of groupsByRank.values()) {
      if (rankGroups.length < 2) {
        continue;
      }

      for (let i = 0; i < rankGroups.length - 1; i++) {
        for (let j = i + 1; j < rankGroups.length; j++) {
          const nextIndexes = [...indexes];
          nextIndexes[rankGroups[i]] -= 1;
          nextIndexes[rankGroups[j]] -= 1;

          if (solve(nextIndexes)) {
            memo.set(key, true);
            return true;
          }
        }
      }
    }

    memo.set(key, false);
    return false;
  };

  return solve(topIndexes);
}

function createDeadlockedLayout() {
  const deck = createDeck();
  const aceOfHearts = deck.find((card) => card.id === 'a-hearts');
  const aceOfDiamonds = deck.find((card) => card.id === 'a-diamonds');

  const groups = groupDeck(deck).map((group) =>
    group.map((card) => ({
      ...card,
      isMatched: true,
      isFlipped: true,
      isMatching: false
    }))
  );

  // Eşleşebilir iki kartı aynı grupta bırakarak hamlesiz durum üret.
  groups[0][0] = {
    ...aceOfHearts,
    isMatched: false,
    isFlipped: false,
    isMatching: false
  };
  groups[0][1] = {
    ...aceOfDiamonds,
    isMatched: false,
    isFlipped: false,
    isMatching: false
  };

  return groups;
}

describe('deste yardımcıları', () => {
  it('benzersiz id değerlerine sahip 52 kartlık deste oluşturur', () => {
    const deck = createDeck();
    const ids = deck.map((card) => card.id);

    expect(deck).toHaveLength(52);
    expect(new Set(ids).size).toBe(52);
    expect(deck.every((card) => card.isMatched === false)).toBe(true);
    expect(deck.every((card) => card.isFlipped === false)).toBe(true);
    expect(deck.every((card) => card.isMatching === false)).toBe(true);
  });

  it('orijinal desteyi değiştirmeden karıştırır ve tüm kartları korur', () => {
    const deck = createDeck();
    const originalIds = deck.map((card) => card.id);
    const shuffled = shuffleDeck(deck);

    expect(shuffled).toHaveLength(52);
    expect(deck.map((card) => card.id)).toEqual(originalIds);

    const shuffledSorted = [...shuffled].map((card) => card.id).sort();
    const originalSorted = [...deck].map((card) => card.id).sort();
    expect(shuffledSorted).toEqual(originalSorted);
  });

  it("kartları 4'er karttan oluşan 13 gruba ayırır", () => {
    const deck = createDeck();
    const groups = groupDeck(deck);

    expect(groups).toHaveLength(13);
    expect(groups.every((group) => group.length === 4)).toBe(true);

    const totalCards = groups.reduce((sum, group) => sum + group.length, 0);
    expect(totalCards).toBe(52);
  });

  it('oynanabilir tam oyun düzenini başlatır', () => {
    const groups = initializeGame();

    expect(groups).toHaveLength(13);
    expect(groups.every((group) => group.length === 4)).toBe(true);

    const flat = groups.flat();
    expect(flat).toHaveLength(52);
    expect(flat.every((card) => typeof card.rank === 'string')).toBe(true);
    expect(flat.every((card) => typeof card.suit === 'string')).toBe(true);
    expect(isLayoutSolvable(groups)).toBe(true);
  });

  it('tekrarlı başlangıçlarda kilitlenmeyen düzenler üretir', () => {
    for (let i = 0; i < 25; i++) {
      const groups = initializeGame();
      expect(isLayoutSolvable(groups)).toBe(true);
    }
  });

  it('hamle kalmadığında kalan kartları yeniden dağıtıp oyunu açar', () => {
    const deadlockedGroups = createDeadlockedLayout();
    expect(hasAvailableMoves(deadlockedGroups)).toBe(false);

    const stabilized = stabilizeGroups(deadlockedGroups);
    expect(hasAvailableMoves(stabilized)).toBe(true);

    const unmatchedBefore = deadlockedGroups.flat().filter((card) => !card.isMatched).length;
    const unmatchedAfter = stabilized.flat().filter((card) => !card.isMatched).length;
    expect(unmatchedAfter).toBe(unmatchedBefore);
  });
});
