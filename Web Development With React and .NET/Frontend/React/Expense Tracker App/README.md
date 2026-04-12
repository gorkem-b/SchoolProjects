# Expense Tracker App

React + Vite ile geliştirilmiş, localStorage kalıcılığı olan bir gider takip uygulaması.

## Özellikler

- Gider ekleme ve silme
- Güncel bakiye, toplam gelir ve toplam gider hesaplaması
- Sayfa yenilense bile verinin korunması (localStorage)
- Boş liste durumu mesajı (`Harcama yok`)
- Responsive (masaüstü + mobil) düzen
- Form doğrulama (boş açıklama, 0/negatif/geçersiz tutar engeli)

## Kullanılan Teknolojiler

- React 19
- Vite 8
- Context API
- Vitest + Testing Library
- ESLint

## Kurulum

```bash
npm install
```

## Geliştirme ve Çalıştırma Komutları

```bash
# development server
npm run dev

# lint kontrolü
npm run lint

# testler
npm run test

# production build
npm run build

# build preview
npm run preview
```

## Proje Yapısı

```text
src/
├── components/
│   ├── expenses/
│   │   ├── ExpenseForm.jsx
│   │   ├── ExpenseItem.jsx
│   │   └── ExpenseList.jsx
│   ├── layout/
│   │   └── Header.jsx
│   └── summary/
│       ├── Balance.jsx
│       └── IncomeExpenseSummary.jsx
├── context/
│   ├── ExpenseContext.jsx
│   ├── ExpenseProvider.jsx
│   └── useExpense.js
├── hooks/
│   └── useLocalStorage.js
├── test/
│   └── setup.js
├── utils/
│   └── formatters.js
├── App.jsx
├── App.test.jsx
├── index.css
└── main.jsx
```

## Mimari Notlar

- Global state, `ExpenseProvider` içinde yönetilir.
- Form state, yalnızca `ExpenseForm` içinde local tutulur.
- Gider kayıtları negatif sayıyla saklanır; böylece `balance = income + expense` formülü sade kalır.
- Hesaplamalar (`income`, `expense`, `balance`) `useMemo` ile optimize edilir.
- Kayıt kimliği için `crypto.randomUUID()` kullanılır.

## Test Kapsamı

`src/App.test.jsx` dosyası ile şu başlıklar doğrulanır:

- Core flow (ekleme, listede sıralama, silme, empty state)
- Validation (boş açıklama, 0/negatif/geçersiz tutar)
- Calculations (income/expense/balance ve sınıf doğrulamaları)
- Persistence (yeniden mount sonrası veri korunumu)
- Responsive/UI (kritik CSS kuralları ve focus-visible)

Manuel kontrol listesi: `MANUAL_TEST_CHECKLIST.md`

## Build Durumu

Son doğrulamalar:

- `npm run lint` ✅
- `npm run test` ✅ (5/5)
- `npm run build` ✅

## Geliştirme Notu

İleride gelir ekleme de desteklenecekse formda işlem tipi (`income` / `expense`) alanı eklenerek aynı state modeliyle genişletilebilir.
