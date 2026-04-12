# Music Player

A modern React + Vite music player with playlist management, progress/seek controls, volume control, and responsive density modes.

## Features

- HTML5 Audio playback with play, pause, next, and previous controls
- Seekable progress bar synced with real audio time updates
- Volume slider synced with the native audio element
- Playlist with active track highlighting and click-to-play selection
- Responsive layout for mobile and desktop
- Global density switch: `normal`, `compact`, `mini`
- Spotify-inspired color palette using Tailwind CSS

## Tech Stack

- React
- Vite
- JavaScript (ES modules)
- Tailwind CSS
- Lucide React (icons)
- HTML5 Audio API
- Context API + React Hooks

## Architecture

The app follows a component-based structure with unidirectional data flow.

1. UI layer: visual components (`Player`, `Playlist`, `SongInfo`, `Controls`, `Progress`, `Volume`)
2. Logic layer: playback state and actions in `PlayerContext`, audio side effects in `useAudio`
3. Data layer: local mock metadata in `src/data/songs.json`

### State Ownership

`PlayerContext` owns shared playback state:

- `currentSongIndex`
- `isPlaying`
- `volume`
- `currentTime`
- `duration`

It also exposes actions:

- `togglePlay`
- `nextSong`
- `prevSong`
- `playSong`
- `seek`
- `setVolume`

## Project Structure

```text
music-player/
├── public/
│   ├── assets/images/       # Cover images
│   └── music/               # MP3 files
├── src/
│   ├── components/
│   │   ├── Player/
│   │   │   ├── Player.jsx
│   │   │   ├── Controls.jsx
│   │   │   ├── Progress.jsx
│   │   │   ├── Volume.jsx
│   │   │   └── SongInfo.jsx
│   │   └── Playlist/
│   │       ├── Playlist.jsx
│   │       └── SongItem.jsx
│   ├── context/
│   │   └── PlayerContext.jsx
│   ├── hooks/
│   │   └── useAudio.js
│   ├── data/
│   │   └── songs.json
│   ├── utils/
│   │   └── formatTime.js
│   ├── App.jsx
│   ├── main.jsx
│   └── index.css
├── index.html
├── package.json
├── tailwind.config.js
└── vite.config.js
```

## How It Works

1. The app loads mock songs from `src/data/songs.json`.
2. `PlayerContext` tracks global playback state.
3. `useAudio` attaches `timeupdate`, `loadedmetadata`, `ended`, and `error` listeners to a single `Audio` instance.
4. UI components subscribe to context values and dispatch actions.
5. Progress and volume sliders update both React state and the underlying audio element.

## Getting Started

1. Install dependencies:

   ```bash
   npm install
   ```

2. Start development server:

   ```bash
   npm run dev
   ```

3. Run lint checks:

   ```bash
   npm run lint
   ```

4. Build for production:

   ```bash
   npm run build
   npm run preview
   ```

## Density Modes

Use the top-right toggle to switch between:

- `normal`: larger spacing and typography
- `compact`: balanced default mode
- `mini`: high information density

The selected mode scales both player and playlist blocks consistently.

## Notes

- Audio error fallback skips to the next track to keep playback flow uninterrupted.
- Event listeners are cleaned up in `useAudio` effects to prevent duplicate subscriptions.
- `formatTime` guards invalid values to keep time labels stable.
