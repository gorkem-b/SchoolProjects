import { useEffect, useState } from 'react'
import { createBook, deleteBook, getBooks } from '../api/bookApi'
import BookCard from '../components/BookCard'
import BookForm from '../components/BookForm'
import EmptyState from '../components/EmptyState'

function getErrorMessage(error) {
  return error?.response?.data || 'An error occurred while processing the request.'
}

function BooksPage() {
  const [books, setBooks] = useState([])
  const [loading, setLoading] = useState(true)
  const [submitting, setSubmitting] = useState(false)
  const [deletingId, setDeletingId] = useState(null)
  const [errorMessage, setErrorMessage] = useState('')

  useEffect(() => {
    let isMounted = true

    getBooks()
      .then((data) => {
        if (isMounted) {
          setBooks(data)
        }
      })
      .catch((error) => {
        if (isMounted) {
          setErrorMessage(getErrorMessage(error))
        }
      })
      .finally(() => {
        if (isMounted) {
          setLoading(false)
        }
      })

    return () => {
      isMounted = false
    }
  }, [])

  async function handleCreateBook(formData) {
    setSubmitting(true)
    setErrorMessage('')

    try {
      const createdBook = await createBook(formData)
      setBooks((current) => [createdBook, ...current])
      return true
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
      return false
    } finally {
      setSubmitting(false)
    }
  }

  async function handleDeleteBook(id) {
    setDeletingId(id)
    setErrorMessage('')

    try {
      await deleteBook(id)
      setBooks((current) => current.filter((book) => book.id !== id))
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    } finally {
      setDeletingId(null)
    }
  }

  return (
    <section className="page">
      <div className="page__header">
        <h1 className="page__title">Books</h1>
        <p className="page__description">
          Add new books to your library and view the task summary for each book.
        </p>
      </div>

      <div className="panel-grid">
        <aside className="panel">
          <div className="panel__header">
            <h2 className="panel__title">Add a new book</h2>
            <p className="panel__description">Save it with title, author, and genre details.</p>
          </div>

          <BookForm onSubmit={handleCreateBook} submitting={submitting} />

          {errorMessage ? <p className="feedback feedback--error">{errorMessage}</p> : null}
        </aside>

        <div className="panel">
          <div className="panel__header">
            <h2 className="panel__title">Library list</h2>
            <p className="panel__description">Total books: {books.length}</p>
          </div>

          {loading ? <p className="helper-text">Loading books...</p> : null}

          {!loading && books.length === 0 ? (
            <EmptyState
              title="No books yet"
              text="Add your first book to start building your library."
            />
          ) : null}

          <div className="book-grid">
            {books.map((book) => (
              <BookCard
                key={book.id}
                book={book}
                onDelete={handleDeleteBook}
                deleting={deletingId === book.id}
              />
            ))}
          </div>
        </div>
      </div>
    </section>
  )
}

export default BooksPage