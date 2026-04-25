import { taskStatusLabels } from '../constants/taskStatuses'

function getStatusClassName(status) {
  return status === 'Completed'
    ? 'status-badge status-badge--completed'
    : 'status-badge status-badge--toread'
}

function BookCard({ book, onDelete, deleting }) {
  return (
    <article className="book-card">
      <div className="book-card__header">
        <div>
          <h3 className="book-card__title">{book.title}</h3>
          <p className="book-card__meta">
            {book.author} · {book.genre}
          </p>
        </div>

        <button
          className="button-danger"
          type="button"
          onClick={() => onDelete(book.id)}
          disabled={deleting}
        >
          Delete
        </button>
      </div>

      {book.tasks.length > 0 ? (
        <ul className="book-card__tasks">
          {book.tasks.map((task) => (
            <li key={task.id}>
              <strong>{task.title}</strong>{' '}
              <span className={getStatusClassName(task.status)}>
                {taskStatusLabels[task.status]}
              </span>
            </li>
          ))}
        </ul>
      ) : (
        <p className="helper-text">No tasks have been added for this book yet.</p>
      )}
    </article>
  )
}

export default BookCard