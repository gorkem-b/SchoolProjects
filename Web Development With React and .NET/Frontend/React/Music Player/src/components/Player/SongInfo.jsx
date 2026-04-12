import { usePlayer } from '../../context/PlayerContext'

const WRAPPER_STYLES = {
  mini: 'gap-2.5 rounded-lg p-2.5',
  compact: 'gap-3 rounded-lg p-3',
  normal: 'gap-3.5 rounded-xl p-3.5',
}

const COVER_STYLES = {
  mini: 'h-14 w-14 rounded-md',
  compact: 'h-[3.9rem] w-[3.9rem] rounded-md',
  normal: 'h-[4.35rem] w-[4.35rem] rounded-lg',
}

const TITLE_STYLES = {
  mini: 'text-base',
  compact: 'text-[1.05rem]',
  normal: 'text-lg',
}

const ARTIST_STYLES = {
  mini: 'text-[11px]',
  compact: 'text-xs',
  normal: 'text-sm',
}

function SongInfo({ density }) {
  const { currentSong } = usePlayer()

  return (
    <section
      className={`flex items-center bg-zinc-950/80 shadow-md ring-1 ring-white/10 ${WRAPPER_STYLES[density]}`}
    >
      <img
        src={currentSong.cover}
        alt={`${currentSong.title} cover`}
        className={`object-cover ${COVER_STYLES[density]}`}
      />
      <div className="min-w-0">
        {/* Keep metadata single-line to avoid layout jumps on narrow screens. */}
        <p className="truncate text-[10px] uppercase tracking-[0.18em] text-spotify-green/80">
          Now Playing
        </p>
        <h1 className={`truncate font-semibold leading-tight text-white ${TITLE_STYLES[density]}`}>
          {currentSong.title}
        </h1>
        <p className={`truncate leading-tight text-zinc-300 ${ARTIST_STYLES[density]}`}>
          {currentSong.artist}
        </p>
      </div>
    </section>
  )
}

export default SongInfo
