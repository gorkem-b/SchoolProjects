import { useState } from 'react'
import { taskStatusOptions } from '../constants/taskStatuses'

function getDefaultBookId(books) {
  return books[0]?.id ? String(books[0].id) : ''
}

function TaskForm({ books, onSubmit, submitting }) {
  const [formData, setFormData] = useState({
    title: '',
    status: 'ToRead',
    bookId: getDefaultBookId(books),
  })

  const selectedBookId =
    formData.bookId && books.some((book) => String(book.id) === formData.bookId)
      ? formData.bookId
      : getDefaultBookId(books)

  function handleChange(event) {
    const { name, value } = event.target
    setFormData((current) => ({
      ...current,
      [name]: value,
    }))
  }

  async function handleSubmit(event) {
    event.preventDefault()

    const isSuccess = await onSubmit({
      title: formData.title,
      status: formData.status,
      bookId: Number(selectedBookId),
    })

    if (isSuccess) {
      setFormData({
        title: '',
        status: 'ToRead',
        bookId: getDefaultBookId(books),
      })
    }
  }

  return (
    <form className="form-grid" onSubmit={handleSubmit}>
      <div className="input-group">
        <label htmlFor="task-title">Task title</label>
        <input
          id="task-title"
          className="input"
          name="title"
          value={formData.title}
          onChange={handleChange}
          placeholder="e.g. Read the first 3 chapters"
          required
        />
      </div>

      <div className="input-group">
        <label htmlFor="task-book">Related book</label>
        <select
          id="task-book"
          className="select"
          name="bookId"
          value={selectedBookId}
          onChange={handleChange}
          required
          disabled={books.length === 0}
        >
          {books.map((book) => (
            <option key={book.id} value={book.id}>
              {book.title}
            </option>
          ))}
        </select>
      </div>

      <div className="input-group">
        <label htmlFor="task-status">Status</label>
        <select
          id="task-status"
          className="select"
          name="status"
          value={formData.status}
          onChange={handleChange}
        >
          {taskStatusOptions.map((option) => (
            <option key={option.value} value={option.value}>
              {option.label}
            </option>
          ))}
        </select>
      </div>

      <button className="button" type="submit" disabled={submitting || books.length === 0}>
        {submitting ? 'Saving...' : 'Add task'}
      </button>
    </form>
  )
}

export default TaskForm