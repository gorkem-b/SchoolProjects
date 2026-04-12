import { useState } from 'react'
import Player from './components/Player/Player'
import Playlist from './components/Playlist/Playlist'
import { PlayerProvider } from './context/PlayerContext'

// Density tokens keep spacing and typography changes consistent across the full UI.
const DENSITY_STYLES = {
  mini: {
    main: 'px-3 py-6 sm:px-5',
    layout: 'max-w-5xl gap-3',
  },
  compact: {
    main: 'px-4 py-7 sm:px-6',
    layout: 'max-w-[68rem] gap-3.5',
  },
  normal: {
    main: 'px-4 py-9 sm:px-8',
    layout: 'max-w-6xl gap-4',
  },
}

function App() {
  const [density, setDensity] = useState('compact')
  const densityClass = DENSITY_STYLES[density]

  return (
    <PlayerProvider>
      <main
        className={`relative min-h-screen overflow-hidden bg-zinc-950 text-zinc-100 ${densityClass.main}`}
      >
        <div className="pointer-events-none absolute inset-0 bg-[radial-gradient(circle_at_20%_15%,rgba(29,185,84,0.25),transparent_35%),radial-gradient(circle_at_80%_85%,rgba(29,185,84,0.16),transparent_40%)]" />

        <div className="relative mx-auto mb-3 flex max-w-6xl justify-end">
          <div className="inline-flex rounded-full border border-white/10 bg-zinc-900/70 p-1 text-xs">
            {/* Simple global density switch for quick readability tuning. */}
            {['normal', 'compact', 'mini'].map((mode) => (
              <button
                key={mode}
                type="button"
                onClick={() => setDensity(mode)}
                className={`rounded-full px-3 py-1 transition ${
                  density === mode ? 'bg-spotify-green text-black' : 'text-zinc-300 hover:bg-zinc-800'
                }`}
              >
                {mode}
              </button>
            ))}
          </div>
        </div>

        <div
          className={`relative mx-auto grid md:grid-cols-[1.15fr_0.95fr] ${densityClass.layout}`}
        >
          <Player density={density} />
          <Playlist density={density} />
        </div>
      </main>
    </PlayerProvider>
  )
}

export default App
