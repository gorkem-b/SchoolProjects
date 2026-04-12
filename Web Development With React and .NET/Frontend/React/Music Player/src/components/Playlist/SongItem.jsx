const ITEM_STYLES = {
  mini: {
    row: 'gap-2 rounded-md px-2 py-1.5',
    cover: 'h-8 w-8',
    title: 'text-[11px]',
    artist: 'text-[10px]',
  },
  compact: {
    row: 'gap-2.5 rounded-md px-2.5 py-1.5',
    cover: 'h-9 w-9',
    title: 'text-xs',
    artist: 'text-[11px]',
  },
  normal: {
    row: 'gap-3 rounded-lg px-3 py-2',
    cover: 'h-10 w-10',
    title: 'text-sm',
    artist: 'text-xs',
  },
}

function SongItem({ song, isActive, onSelect, density }) {
  const styles = ITEM_STYLES[density]

  return (
    <button
      type="button"
      onClick={() => onSelect(song.id)}
      className={`flex w-full items-center text-left transition ${styles.row} ${
        isActive ? 'bg-spotify-green/20 ring-1 ring-spotify-green/60' : 'hover:bg-zinc-800/80'
      }`}
    >
      <img src={song.cover} alt={`${song.title} cover`} className={`rounded object-cover ${styles.cover}`} />
      <div className="min-w-0 flex-1">
        <p className={`truncate font-medium leading-tight text-white ${styles.title}`}>{song.title}</p>
        <p className={`truncate leading-tight text-zinc-400 ${styles.artist}`}>{song.artist}</p>
      </div>
    </button>
  )
}

export default SongItem
