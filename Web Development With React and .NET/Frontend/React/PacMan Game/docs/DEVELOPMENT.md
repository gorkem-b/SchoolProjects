# Development Guide

This document explains how the game is structured and where to implement changes safely.

## Runtime Flow

1. `useKeyboard` captures input and updates `nextDirection`.
2. `useGameLoop` dispatches `APPLY_TICK` at fixed intervals.
3. `gameReducer` computes movement, pellet collection, ghost movement, and state transitions.
4. React re-renders board cells and entities from updated state.

## State Model

`src/context/GameReducer.js` state fields:

- `map`: current mutable map matrix
- `pacmanPosition`: `{ x, y }`
- `pacmanDirection`: active direction
- `nextDirection`: buffered input direction
- `ghosts`: array of ghost positions and metadata
- `score`: integer points
- `gameStatus`: `START | PLAYING | GAME_OVER | WIN`
- `pelletsLeft`: remaining collectible count

## Action Reference

- `START_GAME`: switches state to `PLAYING`
- `SET_NEXT_DIRECTION`: buffers user direction
- `APPLY_TICK`: runs one simulation step
- `RESET_GAME`: re-creates initial state

## Where To Extend

### Add new maps

- Edit `src/utils/mapLayout.js`
- Keep `1` as wall, `2` as pellet, `3` as power pellet

### Adjust speed and scale

- Edit `src/utils/constants.js`:
  - `GAME_SPEED`
  - `CELL_SIZE`

### Improve ghost AI

- Update `moveGhosts` in `src/context/GameReducer.js`
- Keep movement constrained by `isWall` checks

### Add extra UI panels

- Add components under `src/components/UI`
- Keep presentation-only logic inside UI components

## Testing Guidance

Run full checks before merge:

```bash
npm run lint
npm test -- --runInBand
npm run build
```

Recommended additional tests:

- Ghost collision edge cases
- Win condition transitions
- Direction buffering behavior at intersections

## Coding Guidelines

- Keep simulation logic inside reducer/helpers.
- Keep React hooks focused on side effects (input/timers).
- Prefer small pure utility functions for movement and tile checks.
- Add concise comments only around non-obvious behavior.
