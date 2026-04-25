const fallbackPoster =
  'https://placehold.co/400x600/0f172a/e2e8f0?text=No+Poster'

function TaskItem({ task, onUpdate, onDelete, isBusy }) {
  const handleCategoryChange = async (event) => {
    await onUpdate(task.id, {
      ...task,
      category: event.target.value,
    })
  }

  const handleWatchedChange = async (event) => {
    await onUpdate(task.id, {
      ...task,
      isWatched: event.target.checked,
    })
  }

  return (
    <article className="panel task-card">
      <img
        className="task-poster"
        src={task.posterUrl || fallbackPoster}
        alt={`${task.title} poster`}
      />

      <div>
        <h2 className="card-title">{task.title}</h2>
        <p className="muted-text">{task.year || 'Unknown year'}</p>
      </div>

      {task.description ? <p>{task.description}</p> : null}

      <div className="task-controls">
        <label className="checkbox-row">
          <input
            type="checkbox"
            checked={task.isWatched}
            onChange={handleWatchedChange}
            disabled={isBusy}
          />
          Mark as watched
        </label>

        <select
          className="select-input"
          value={task.category}
          onChange={handleCategoryChange}
          disabled={isBusy}
        >
          <option value="Watchlist">Watchlist</option>
          <option value="Favorite">Favorite</option>
        </select>

        <button
          type="button"
          className="danger-button"
          onClick={() => onDelete(task.id)}
          disabled={isBusy}
        >
          Delete
        </button>
      </div>
    </article>
  )
}

export default TaskItem
