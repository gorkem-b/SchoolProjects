# Manual Test Checklist

## Core Flows

- [x] Add a new expense with valid description and amount.
- [x] Verify new expense appears at top of transaction list.
- [x] Delete an expense and verify it is removed from the list.
- [x] Verify empty state text `Harcama yok` appears when all expenses are deleted.

## Validation

- [x] Submit with empty description and verify error message appears.
- [x] Submit with `0` amount and verify error message appears.
- [x] Submit with negative amount and verify error message appears.
- [x] Submit with non-numeric amount and verify error message appears.

## Calculations

- [x] Verify `Income` remains `₺0.00` when only expenses are added.
- [x] Verify `Expense` total updates with each new expense.
- [x] Verify `Current Balance` decreases correctly after each expense.
- [x] Verify positive/negative color coding is applied correctly.

## Persistence

- [x] Add 2-3 expenses, refresh the page, and verify data persists.
- [x] Delete one persisted item, refresh again, and verify deletion persists.

## Responsive & UI

- [x] Verify layout on desktop (two-column grid).
- [x] Verify layout on tablet/mobile (single-column stack).
- [x] Verify buttons and inputs are keyboard-focus visible.

## Build

- [x] Run `npm run build` and verify successful production build.
