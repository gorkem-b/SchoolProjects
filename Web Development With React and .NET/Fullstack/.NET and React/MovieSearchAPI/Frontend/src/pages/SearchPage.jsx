import { useState } from 'react'
import axiosClient from '../api/axiosClient'
import MovieCard from '../components/MovieCard'

function SearchPage() {
  const [query, setQuery] = useState('')
  const [description, setDescription] = useState('')
  const [movies, setMovies] = useState([])
  const [isSearching, setIsSearching] = useState(false)
  const [isSubmitting, setIsSubmitting] = useState(false)
  const [error, setError] = useState('')
  const [successMessage, setSuccessMessage] = useState('')

  const handleSearch = async (event) => {
    event.preventDefault()

    const trimmedQuery = query.trim()

    if (!trimmedQuery) {
      setError('Please enter a movie title.')
      setSuccessMessage('')
      return
    }

    setIsSearching(true)
    setError('')
    setSuccessMessage('')

    try {
      const response = await axiosClient.get('/movies/search', {
        params: { title: trimmedQuery },
      })

      setMovies(response.data)
      if (response.data.length === 0) {
        setSuccessMessage('No results found for that search.')
      }
    } catch (requestError) {
      const message =
        requestError.response?.data?.detail ||
        requestError.response?.data?.message ||
        'Movie search failed. Check your OMDb configuration.'
      setError(message)
      setMovies([])
    } finally {
      setIsSearching(false)
    }
  }

  const handleAddTask = async (movie, category) => {
    setIsSubmitting(true)
    setError('')
    setSuccessMessage('')

    try {
      await axiosClient.post('/tasks', {
        title: movie.title,
        description: description.trim() || null,
        category,
        isWatched: false,
        year: movie.year || null,
        posterUrl: movie.posterUrl || null,
      })

      setSuccessMessage(`${movie.title} added to ${category.toLowerCase()}.`)
    } catch (requestError) {
      const message =
        requestError.response?.data?.message || 'Could not save the movie task.'
      setError(message)
    } finally {
      setIsSubmitting(false)
    }
  }

  return (
    <section>
      <header className="page-header">
        <h1 className="page-title">Search movies</h1>
        <p className="page-description">
          Search OMDb, then save results as watchlist or favorite tasks in your local SQL
          Server database.
        </p>
      </header>

      <div className="panel">
        <form className="search-form" onSubmit={handleSearch}>
          <input
            className="text-input"
            type="text"
            placeholder="Search for a movie title"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
          />
          <button type="submit" className="primary-button" disabled={isSearching}>
            {isSearching ? 'Searching...' : 'Search'}
          </button>
        </form>

        <textarea
          className="text-area"
          placeholder="Optional note for saved tasks"
          value={description}
          onChange={(event) => setDescription(event.target.value)}
        />
      </div>

      {error ? <p className="status-message error">{error}</p> : null}
      {successMessage ? <p className="status-message success">{successMessage}</p> : null}

      {movies.length > 0 ? (
        <div className="grid">
          {movies.map((movie) => (
            <MovieCard
              key={movie.imdbId}
              movie={movie}
              onAdd={handleAddTask}
              isSubmitting={isSubmitting}
            />
          ))}
        </div>
      ) : (
        <div className="empty-state">Search for a title to see movie results here.</div>
      )}
    </section>
  )
}

export default SearchPage
