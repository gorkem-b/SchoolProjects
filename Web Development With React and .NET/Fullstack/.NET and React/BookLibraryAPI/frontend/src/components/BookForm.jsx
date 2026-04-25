import { useState } from 'react'

const initialFormState = {
  title: '',
  author: '',
  genre: '',
}

function BookForm({ onSubmit, submitting }) {
  const [formData, setFormData] = useState(initialFormState)

  function handleChange(event) {
    const { name, value } = event.target
    setFormData((current) => ({
      ...current,
      [name]: value,
    }))
  }

  async function handleSubmit(event) {
    event.preventDefault()

    const isSuccess = await onSubmit(formData)
    if (isSuccess) {
      setFormData(initialFormState)
    }
  }

  return (
    <form className="form-grid" onSubmit={handleSubmit}>
      <div className="input-group">
        <label htmlFor="book-title">Book title</label>
        <input
          id="book-title"
          className="input"
          name="title"
          value={formData.title}
          onChange={handleChange}
          placeholder="e.g. Les Miserables"
          required
        />
      </div>

      <div className="input-group">
        <label htmlFor="book-author">Author</label>
        <input
          id="book-author"
          className="input"
          name="author"
          value={formData.author}
          onChange={handleChange}
          placeholder="e.g. Victor Hugo"
          required
        />
      </div>

      <div className="input-group">
        <label htmlFor="book-genre">Genre</label>
        <input
          id="book-genre"
          className="input"
          name="genre"
          value={formData.genre}
          onChange={handleChange}
          placeholder="e.g. Novel"
          required
        />
      </div>

      <button className="button" type="submit" disabled={submitting}>
        {submitting ? 'Saving...' : 'Add book'}
      </button>
    </form>
  )
}

export default BookForm