import { useState, useCallback, useEffect, useRef } from 'react';
import { initializeGame, stabilizeGroups } from '../utils/deck';

const MATCH_SCORE = 10;
const MATCH_ANIMATION_DURATION = 450;

export const useGame = () => {
  const [cardGroups, setCardGroups] = useState(() => initializeGame());
  const [activeSelection, setActiveSelection] = useState([]);
  const [moves, setMoves] = useState(0);
  const [score, setScore] = useState(0);
  const [isLocked, setIsLocked] = useState(false);
  const [gameOver, setGameOver] = useState(false);

  // Ref'ler, ardışık hızlı tıklamalarda stale state sorununu önler.
  const cardGroupsRef = useRef(cardGroups);
  const activeSelectionRef = useRef(activeSelection);
  const isLockedRef = useRef(isLocked);

  useEffect(() => {
    cardGroupsRef.current = cardGroups;
  }, [cardGroups]);

  useEffect(() => {
    activeSelectionRef.current = activeSelection;
  }, [activeSelection]);

  useEffect(() => {
    isLockedRef.current = isLocked;
  }, [isLocked]);

  // Tüm kartlar eşleştiyse oyun biter.
  const checkGameOver = useCallback((groups) => {
    return groups.every(group => group.every(card => card.isMatched));
  }, []);

  // Bir gruptaki oynanabilir üst kartı döndürür.
  // Üst kart açık durumdaysa aynı gruptan ikinci kez seçim yapılmasına izin vermez.
  const getTopPlayableCard = useCallback((group) => {
    for (let i = group.length - 1; i >= 0; i--) {
      if (!group[i].isMatched) {
        if (group[i].isFlipped) {
          return null;
        }
        return { card: group[i], index: i };
      }
    }
    return null;
  }, []);

  // Tek kart seçim akışını yönetir.
  const handleCardClick = useCallback((groupIndex) => {
    // Eşleşme kontrolü sürerken yeni tıklama alınmaz.
    if (isLockedRef.current || activeSelectionRef.current.length === 2) {
      return;
    }

    // Tıklanan gruptan yalnızca üstteki oynanabilir kart alınır.
    const currentGroups = cardGroupsRef.current;
    const topCardInfo = getTopPlayableCard(currentGroups[groupIndex]);
    
    // Kart yoksa veya aynı gruptan tekrar seçim yapılmaya çalışılıyorsa işlemi sonlandır.
    if (!topCardInfo || activeSelectionRef.current.some(sel => sel.groupIndex === groupIndex)) {
      return;
    }

    const { card, index: actualCardIndex } = topCardInfo;
    
    // Seçilen kartı aç.
    setCardGroups(prevGroups => {
      const newGroups = [...prevGroups];
      newGroups[groupIndex] = [...newGroups[groupIndex]];
      newGroups[groupIndex][actualCardIndex] = {
        ...newGroups[groupIndex][actualCardIndex],
        isFlipped: true
      };
      cardGroupsRef.current = newGroups;
      return newGroups;
    });

    // Seçimi geçici listeye ekle.
    const newSelection = [
      ...activeSelectionRef.current,
      { 
        groupIndex, 
        cardIndex: actualCardIndex, 
        card 
      }
    ];
    
    setActiveSelection(newSelection);
    activeSelectionRef.current = newSelection;

    // İki kart tamamlandığında hamle say ve karşılaştır.
    if (newSelection.length === 2) {
      setIsLocked(true);
      isLockedRef.current = true;
      setMoves(prevMoves => prevMoves + 1);

      const [first, second] = newSelection;
      
      // Eşleşme ölçütü: rank eşitliği.
      if (first.card.rank === second.card.rank) {
        // Eşleşen kartları kısa süreli başarı animasyonuna al.
        setCardGroups(prevGroups => {
          const newGroups = [...prevGroups];
          
          // İlk kartı animasyonlu işaretle.
          newGroups[first.groupIndex] = [...newGroups[first.groupIndex]];
          newGroups[first.groupIndex][first.cardIndex] = {
            ...newGroups[first.groupIndex][first.cardIndex],
            isMatching: true
          };
          
          // İkinci kartı animasyonlu işaretle.
          newGroups[second.groupIndex] = [...newGroups[second.groupIndex]];
          newGroups[second.groupIndex][second.cardIndex] = {
            ...newGroups[second.groupIndex][second.cardIndex],
            isMatching: true
          };

          cardGroupsRef.current = newGroups;

          return newGroups;
        });

        setScore(prevScore => prevScore + MATCH_SCORE);

        setTimeout(() => {
          setCardGroups(prevGroups => {
            const newGroups = [...prevGroups];

            // Eşleşmeyi kalıcılaştır.
            newGroups[first.groupIndex] = [...newGroups[first.groupIndex]];
            newGroups[first.groupIndex][first.cardIndex] = {
              ...newGroups[first.groupIndex][first.cardIndex],
              isMatching: false,
              isMatched: true
            };

            newGroups[second.groupIndex] = [...newGroups[second.groupIndex]];
            newGroups[second.groupIndex][second.cardIndex] = {
              ...newGroups[second.groupIndex][second.cardIndex],
              isMatching: false,
              isMatched: true
            };

            // Hamle kalmadıysa kalan kartları otomatik yeniden düzenle.
            const stabilizedGroups = stabilizeGroups(newGroups);
            cardGroupsRef.current = stabilizedGroups;

            // Tüm kartlar eşleştiyse oyunu bitir.
            if (checkGameOver(stabilizedGroups)) {
              setGameOver(true);
            }

            return stabilizedGroups;
          });

          setActiveSelection([]);
          activeSelectionRef.current = [];
          setIsLocked(false);
          isLockedRef.current = false;
        }, MATCH_ANIMATION_DURATION);
      } else {
        // Eşleşmeyen kartları geri kapat.
        setTimeout(() => {
          setCardGroups(prevGroups => {
            const newGroups = [...prevGroups];
            
            // İlk kartı kapat.
            newGroups[first.groupIndex] = [...newGroups[first.groupIndex]];
            newGroups[first.groupIndex][first.cardIndex] = {
              ...newGroups[first.groupIndex][first.cardIndex],
              isFlipped: false
            };
            
            // İkinci kartı kapat.
            newGroups[second.groupIndex] = [...newGroups[second.groupIndex]];
            newGroups[second.groupIndex][second.cardIndex] = {
              ...newGroups[second.groupIndex][second.cardIndex],
              isFlipped: false
            };
            // Hamle kalmadıysa kilitlenmeyi önlemek için düzeni stabilize et.
            const stabilizedGroups = stabilizeGroups(newGroups);
            cardGroupsRef.current = stabilizedGroups;
            
            return stabilizedGroups;
          });

          setActiveSelection([]);
          activeSelectionRef.current = [];
          setIsLocked(false);
          isLockedRef.current = false;
        }, 1000);
      }
    }
  }, [getTopPlayableCard, checkGameOver]);

  // Tüm oyun durumunu başlangıç haline döndür.
  const restartGame = useCallback(() => {
    setCardGroups(initializeGame());
    setActiveSelection([]);
    setMoves(0);
    setScore(0);
    setIsLocked(false);
    setGameOver(false);
  }, []);

  return {
    cardGroups,
    activeSelection,
    moves,
    score,
    isLocked,
    gameOver,
    handleCardClick,
    restartGame
  };
};
