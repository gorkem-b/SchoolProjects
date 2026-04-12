# Playing Cards Matching Game

## Proje Özeti

Bu proje React ile geliştirilmiş 52 kartlık bir eşleştirme oyunudur. Kartlar 13 gruba dağıtılır ve oyuncu her hamlede farklı iki gruptan üstteki kartları açar. Seçilen kartların rütbe değeri eşitse kartlar eşleşmiş kabul edilir, değilse kısa bir gecikmeden sonra tekrar kapanır.

Oyunun hedefi tüm kartları eşleştirmektir. Her iki kart seçiminde hamle sayısı artar, her doğru eşleşme puana +10 ekler.

## Son Sürümde Öne Çıkan Davranışlar 

1. Başlangıç deste yerleşimi çözülebilir şekilde üretilir.
2. Oyun sırasında hamle kalmazsa (üstte eşleşme yoksa) kalan eşleşmemiş kartlar otomatik stabilize edilerek yeniden dağıtılır.
3. Bu mekanizma sayesinde sona doğru kitlenme durumları engellenir.

## Oyun Akışı (Sözde Kod)

```text
BAŞLAT
	kartGrupları = initializeGame()   // çözülebilir başlangıç düzeni
	aktifSeçim = []
	hamle = 0
	skor = 0
	oyunBitti = false

KART_TIKLANDI(grupIndex)
	EĞER oyun kilitliyse ÇIK
	EĞER aktifSeçim 2 ise ÇIK

	üstKart = gruptaki oynanabilir üst kart
	EĞER üstKart yoksa ÇIK
	EĞER aynı gruptan seçim varsa ÇIK

	üstKartı aç
	aktifSeçim'e ekle

	EĞER aktifSeçim 2 olduysa
		hamle += 1
		ilk, ikinci = aktifSeçim

		EĞER ilk.rank == ikinci.rank
			eşleşme animasyonu
			kartları matched yap
			skor += 10
		DEĞİLSE
			iki kartı geri kapat
		SON

		kartGrupları = stabilizeGroups(kartGrupları)
		EĞER tüm kartlar matched ise oyunBitti = true
		aktifSeçim = []
	SON

YENİDEN_BAŞLAT
	tüm oyun durumunu sıfırla
```

## Kullanılan Teknolojiler

- React 19
- JavaScript
- CSS
- React Testing Library

## Proje Yapısı

```text
src/
	components/
		Card.jsx
		CardGroup.jsx
		GameBoard.jsx
		GameOverScreen.jsx
		Header.jsx
	hooks/
		useGame.js
	utils/
		deck.js
		deck.test.js
	test/
		setup.js
	App.jsx
	App.test.jsx
	App.css
	index.css
	main.jsx
public/
	...
```

## Mimari Özeti

1. Sunum Katmanı
Card, CardGroup, GameBoard, Header ve GameOverScreen bileşenleri görsel sunum ve kullanıcı etkileşimini yönetir.

2. Oyun Durumu Katmanı
useGame hook'u seçim akışı, skor/hamle hesaplama, kilit yönetimi, eşleşme kontrolü, stabilizasyon ve oyun bitiş mantığını tek noktada toplar.

3. Yardımcı Fonksiyon Katmanı
deck.js dosyası kart üretimi, karıştırma, başlangıç dağıtımı, mevcut hamle kontrolü ve deadlock önleyici stabilizasyon işlevlerini içerir.

4. Test Katmanı
deck.test.js kart üretim/stabilizasyon kurallarını, App.test.jsx temel kullanıcı akışını doğrular.