import Controls from './Controls'
import Progress from './Progress'
import SongInfo from './SongInfo'
import Volume from './Volume'

const CONTAINER_STYLES = {
  mini: 'space-y-3 rounded-xl p-3 shadow-md',
  compact: 'space-y-3.5 rounded-xl p-3.5 shadow-lg',
  normal: 'space-y-4 rounded-2xl p-4 shadow-xl',
}

function Player({ density }) {
  return (
    <section
      className={`border border-white/10 bg-zinc-900/80 backdrop-blur ${CONTAINER_STYLES[density]}`}
    >
      <SongInfo density={density} />
      <Progress density={density} />
      <Controls density={density} />
      <Volume density={density} />
    </section>
  )
}

export default Player
