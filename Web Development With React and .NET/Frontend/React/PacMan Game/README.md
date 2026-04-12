# Pacman Arena

A grid-based Pacman game built with React and Vite.

![Pacman Arena Logo](./public/logo.svg)

## Overview

Pacman Arena is a lightweight arcade clone designed to showcase clean React architecture for game-like state updates.

## Features

- Grid board rendered with CSS Grid
- Keyboard controls with buffered direction input
- Fixed-timestep game loop driven by `setInterval`
- Centralized game state with Context + `useReducer`
- Pellet and power-pellet scoring system
- Ghost movement with wall-aware path selection
- End states: `START`, `PLAYING`, `GAME_OVER`, `WIN`
- Automated validation with lint, tests, and production build

## Tech Stack

- React 19
- Vite 8
- JavaScript (ES Modules)
- CSS Modules
- Jest + React Testing Library
- ESLint 9

## Project Structure

```text
PacMan Game/
+-- public/
�   +-- logo.svg
+-- docs/
�   +-- DEVELOPMENT.md
+-- src/
�   +-- components/
�   �   +-- Entity/
�   �   +-- GameBoard/
�   �   +-- Grid/
�   �   +-- UI/
�   +-- context/
�   +-- hooks/
�   +-- utils/
�   +-- App.jsx
�   +-- App.css
�   +-- index.css
�   +-- main.jsx
+-- babel.config.cjs
+-- jest.config.cjs
+-- eslint.config.js
+-- index.html
+-- package.json
+-- README.md
```

## Quick Start

1. Install dependencies:

```bash
npm install
```

1. Start development server:

```bash
npm run dev
```

1. Run lint checks:

```bash
npm run lint
```

1. Run tests:

```bash
npm test
```

1. Build production bundle:

```bash
npm run build
```

## Controls

- Arrow keys: move Pacman
- Enter: start game on start screen
- Enter: restart after game over or win

## Architecture Summary

- `src/context/GameContext.jsx` provides global game state and dispatch.
- `src/context/GameReducer.js` executes the simulation step and game state transitions.
- `src/hooks/useKeyboard.js` maps keyboard events to reducer actions.
- `src/hooks/useGameLoop.js` emits fixed-interval tick actions while playing.
- `src/utils/` contains pure movement and map utilities.

For deeper implementation notes, see [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md).

## Testing

Current tests include:

- Reducer movement and score updates
- Pellet consumption behavior
- Start screen rendering snapshot

Run all checks together:

```bash
npm run lint
npm test -- --runInBand
npm run build
```

## License

This project is for educational and personal development purposes.
