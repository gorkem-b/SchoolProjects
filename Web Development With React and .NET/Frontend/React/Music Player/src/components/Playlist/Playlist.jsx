import { usePlayer } from '../../context/PlayerContext'
import SongItem from './SongItem'

// Shared sizing map keeps list rows and the container visually balanced per density mode.
const PLAYLIST_STYLES = {
  mini: {
    container: 'rounded-xl p-2.5 shadow-md',
    header: 'mb-1.5',
    title: 'text-xs',
    subtitle: 'text-[11px]',
    list: 'space-y-1',
  },
  compact: {
    container: 'rounded-xl p-3 shadow-md',
    header: 'mb-2',
    title: 'text-sm',
    subtitle: 'text-xs',
    list: 'space-y-1.5',
  },
  normal: {
    container: 'rounded-2xl p-4 shadow-lg',
    header: 'mb-2.5',
    title: 'text-base',
    subtitle: 'text-sm',
    list: 'space-y-2',
  },
}

function Playlist({ density }) {
  const { songs, currentSongIndex, playSong } = usePlayer()
  const styles = PLAYLIST_STYLES[density]

  return (
    <section className={`border border-white/10 bg-zinc-900/75 ${styles.container}`}>
      <header className={`${styles.header} px-1`}>
        <h2 className={`font-semibold tracking-wide text-white ${styles.title}`}>Playlist</h2>
        <p className={`text-zinc-400 ${styles.subtitle}`}>Randomly generated mock songs</p>
      </header>
      <div className={styles.list}>
        {songs.map((song, index) => (
          <SongItem
            key={song.id}
            song={song}
            isActive={currentSongIndex === index}
            onSelect={playSong}
            density={density}
          />
        ))}
      </div>
    </section>
  )
}

export default Playlist
