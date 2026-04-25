import { useState } from 'react'
import { taskStatusLabels, taskStatusOptions } from '../constants/taskStatuses'

function getStatusClassName(status) {
  return status === 'Completed'
    ? 'status-badge status-badge--completed'
    : 'status-badge status-badge--toread'
}

function TaskItem({ task, onSave, onDelete, saving, deleting }) {
  const [title, setTitle] = useState(task.title)
  const [status, setStatus] = useState(task.status)

  async function handleSubmit(event) {
    event.preventDefault()
    await onSave(task.id, { title, status })
  }

  return (
    <article className="task-item">
      <div className="task-item__header">
        <div>
          <h3 className="task-item__title">{task.title}</h3>
          <p className="task-item__book">Book: {task.bookTitle}</p>
          <p className="task-item__meta">
            Current status:{' '}
            <span className={getStatusClassName(task.status)}>
              {taskStatusLabels[task.status]}
            </span>
          </p>
        </div>

        <button
          className="button-danger"
          type="button"
          onClick={() => onDelete(task.id)}
          disabled={deleting}
        >
          Delete
        </button>
      </div>

      <form className="task-item__body" onSubmit={handleSubmit}>
        <div className="task-item__controls">
          <input
            className="input"
            value={title}
            onChange={(event) => setTitle(event.target.value)}
            required
          />

          <select
            className="select"
            value={status}
            onChange={(event) => setStatus(event.target.value)}
          >
            {taskStatusOptions.map((option) => (
              <option key={option.value} value={option.value}>
                {option.label}
              </option>
            ))}
          </select>
        </div>

        <div className="task-item__actions">
          <button className="button-secondary" type="submit" disabled={saving}>
            {saving ? 'Saving...' : 'Update'}
          </button>
        </div>
      </form>
    </article>
  )
}

export default TaskItem