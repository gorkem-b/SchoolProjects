// Kart üretimi, dağıtımı ve kilitlenme önleme yardımcıları.

// Standart deste sabitleri.
const SUITS = ['Hearts', 'Diamonds', 'Clubs', 'Spades'];
const RANKS = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];

/**
 * Standart 52 kartlık bir deste oluşturur.
 * @returns {Array} Kart nesnelerinden oluşan dizi
 */
export function createDeck() {
  const deck = [];
  for (let suit of SUITS) {
    for (let rank of RANKS) {
      deck.push({
        id: `${rank}-${suit}`.toLowerCase().replace(/\s/g, ''),
        rank: rank,
        suit: suit,
        isMatched: false,
        isFlipped: false,
        isMatching: false
      });
    }
  }
  return deck;
}

/**
 * Fisher-Yates algoritması ile diziyi karıştırır.
 * @param {Array} array - Karıştırılacak dizi
 * @returns {Array} Karıştırılmış dizi
 */
export function shuffleDeck(array) {
  const shuffled = [...array];
  for (let i = shuffled.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [shuffled[i], shuffled[j]] = [shuffled[j], shuffled[i]];
  }
  return shuffled;
}

/**
 * Düz kart dizisini 13 gruba ayırır (grup başına 4 kart).
 * @param {Array} deck - 52 kartlık dizi
 * @returns {Array} Her biri 4 kart içeren 13 grupluk dizi
 */
export function groupDeck(deck) {
  const groups = [];
  for (let i = 0; i < 13; i++) {
    groups.push(deck.slice(i * 4, (i + 1) * 4));
  }
  return groups;
}

/**
 * Ağırlıklı olasılıkla bir indeks seçer.
 * @param {number[]} indices - Aday indeksler
 * @param {number[]} weights - Adayların ağırlıkları
 * @returns {number} Seçilen indeks
 */
function pickWeightedIndex(indices, weights) {
  const totalWeight = weights.reduce((sum, weight) => sum + weight, 0);
  let random = Math.random() * totalWeight;

  for (let i = 0; i < indices.length; i++) {
    random -= weights[i];
    if (random <= 0) {
      return indices[i];
    }
  }

  return indices[indices.length - 1];
}

/**
 * Her adımda kartları iki farklı gruba dağıtacak grup çiftlerini üretir.
 * Üretilen çiftler, daha sonra aynı rank çiftlerinin hangi gruplara gideceğini belirler.
 * @param {number[]} capacities - Grup başına kart sayısı
 * @returns {Array<[number, number]>} Grup indeks çiftleri
 */
function buildGroupPairings(capacities) {
  const totalCards = capacities.reduce((sum, count) => sum + count, 0);
  const totalPairs = totalCards / 2;
  const maxAttempts = 1500;

  for (let attempt = 0; attempt < maxAttempts; attempt++) {
    const remainingSlots = [...capacities];
    const pairings = [];
    let isValid = true;

    for (let pairIndex = 0; pairIndex < totalPairs; pairIndex++) {
      const candidates = remainingSlots
        .map((slots, groupIndex) => ({ slots, groupIndex }))
        .filter(({ slots }) => slots > 0)
        .map(({ groupIndex }) => groupIndex);

      if (candidates.length < 2) {
        isValid = false;
        break;
      }

      const firstGroup = pickWeightedIndex(
        candidates,
        candidates.map((groupIndex) => remainingSlots[groupIndex])
      );

      const secondCandidates = candidates.filter((groupIndex) => groupIndex !== firstGroup);
      const secondGroup = pickWeightedIndex(
        secondCandidates,
        secondCandidates.map((groupIndex) => remainingSlots[groupIndex])
      );

      remainingSlots[firstGroup] -= 1;
      remainingSlots[secondGroup] -= 1;
      pairings.push([firstGroup, secondGroup]);
    }

    if (isValid && remainingSlots.every((slots) => slots === 0)) {
      return pairings;
    }
  }

  throw new Error('Çözülebilir kart yerleşimi üretilemedi.');
}

/**
 * Kartları rank değerine göre ikili çiftlere ayırır.
 * @param {Array} cards - Çiftlenecek kartlar
 * @returns {Array<[Object, Object]>} Eş rank çiftleri
 */
function buildRankPairs(cards) {
  const cardsByRank = new Map();

  for (const card of cards) {
    const existing = cardsByRank.get(card.rank) || [];
    existing.push(card);
    cardsByRank.set(card.rank, existing);
  }

  const pairs = [];
  for (const cardsOfRank of cardsByRank.values()) {
    const shuffledRankCards = shuffleDeck(cardsOfRank);

    if (shuffledRankCards.length % 2 !== 0) {
      throw new Error('Rank bazlı kart sayısı tek olamaz.');
    }

    for (let i = 0; i < shuffledRankCards.length; i += 2) {
      pairs.push([shuffledRankCards[i], shuffledRankCards[i + 1]]);
    }
  }

  return shuffleDeck(pairs);
}

/**
 * Her grup için mevcut üst oynanabilir kartı bulur.
 * @param {Array[]} groups - Kart grupları
 * @returns {Array<{groupIndex:number, card:Object}>} Üst kartlar
 */
function getTopCards(groups) {
  const topCards = [];

  for (let groupIndex = 0; groupIndex < groups.length; groupIndex++) {
    const group = groups[groupIndex];
    for (let i = group.length - 1; i >= 0; i--) {
      if (!group[i].isMatched) {
        if (!group[i].isFlipped) {
          topCards.push({ groupIndex, card: group[i] });
        }
        break;
      }
    }
  }

  return topCards;
}

/**
 * Üstte görünen kartlar arasında en az bir eşleşme var mı kontrol eder.
 * @param {Array[]} groups - Kart grupları
 * @returns {boolean} Eşleşme varsa true
 */
export function hasAvailableMoves(groups) {
  const countsByRank = new Map();
  const topCards = getTopCards(groups);

  for (const { card } of topCards) {
    const current = countsByRank.get(card.rank) || 0;
    const next = current + 1;
    if (next >= 2) {
      return true;
    }
    countsByRank.set(card.rank, next);
  }

  return false;
}

/**
 * Hamle kalmadığında yalnızca eşleşmemiş kartları yeniden dağıtır.
 * Eşleşmiş kartlar korunur, açık/animasyon durumları normalize edilir.
 * @param {Array[]} groups - Mevcut kart grupları
 * @returns {Array[]} Stabil hale getirilmiş kart grupları
 */
export function stabilizeGroups(groups) {
  const isGameOver = groups.every((group) => group.every((card) => card.isMatched));
  if (isGameOver || hasAvailableMoves(groups)) {
    return groups;
  }

  const unmatchedCards = [];
  const matchedCards = [];

  for (const group of groups) {
    for (const card of group) {
      if (card.isMatched) {
        matchedCards.push({
          ...card,
          isMatched: true,
          isMatching: false,
          isFlipped: true
        });
      } else {
        unmatchedCards.push({
          ...card,
          isMatched: false,
          isMatching: false,
          isFlipped: false
        });
      }
    }
  }

  if (unmatchedCards.length === 0 || unmatchedCards.length % 2 !== 0) {
    return initializeGame();
  }

  const groupCount = groups.length;
  const targetActiveGroups = Math.min(groupCount, Math.max(2, Math.ceil(unmatchedCards.length / 4)));
  const capacities = Array(groupCount).fill(0);
  for (let i = 0; i < unmatchedCards.length; i++) {
    capacities[i % targetActiveGroups] += 1;
  }

  let groupPairings;
  let rankPairs;

  try {
    groupPairings = buildGroupPairings(capacities);
    rankPairs = buildRankPairs(unmatchedCards);
  } catch {
    return initializeGame();
  }

  const unmatchedByGroup = Array.from({ length: groupCount }, () => []);
  for (let i = 0; i < rankPairs.length; i++) {
    const [firstCard, secondCard] = rankPairs[i];
    const [firstGroup, secondGroup] = groupPairings[i];

    unmatchedByGroup[firstGroup].push(firstCard);
    unmatchedByGroup[secondGroup].push(secondCard);
  }

  const matchedPool = shuffleDeck(matchedCards);

  return groups.map((group, groupIndex) => {
    const unmatchedGroupCards = unmatchedByGroup[groupIndex].map((card) => ({
      ...card,
      isMatched: false,
      isMatching: false,
      isFlipped: false
    }));

    const matchedNeeded = group.length - unmatchedGroupCards.length;
    const matchedGroupCards = matchedPool.splice(0, matchedNeeded).map((card) => ({
      ...card,
      isMatched: true,
      isMatching: false,
      isFlipped: true
    }));

    return [...unmatchedGroupCards, ...matchedGroupCards];
  });
}

/**
 * Oyunun başlangıç durumunu oluşturur.
 * Yerleşim geri-sarım yaklaşımıyla kurulur; her adımda eş rank kartlar
 * farklı gruplara yerleştirilir. Bu sayede başlangıç düzeni çözülebilir olur.
 * @returns {Array} Her biri 4 kart içeren 13 grupluk dizi
 */
export function initializeGame() {
  const deck = createDeck();
  const rankPairs = buildRankPairs(deck);
  const groupPairings = buildGroupPairings(Array(13).fill(4));
  const groups = Array.from({ length: 13 }, () => []);

  for (let i = 0; i < rankPairs.length; i++) {
    const [firstCard, secondCard] = rankPairs[i];
    const [firstGroup, secondGroup] = groupPairings[i];

    groups[firstGroup].push({ ...firstCard });
    groups[secondGroup].push({ ...secondCard });
  }

  return groups;
}