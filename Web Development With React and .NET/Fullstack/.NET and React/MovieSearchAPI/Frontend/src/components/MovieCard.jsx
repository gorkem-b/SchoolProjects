import { useState } from 'react'

const fallbackPoster =
  'https://placehold.co/400x600/0f172a/e2e8f0?text=No+Poster'

function MovieCard({ movie, onAdd, isSubmitting }) {
  const [category, setCategory] = useState('Watchlist')

  const handleAdd = () => {
    onAdd(movie, category)
  }

  return (
    <article className="panel movie-card">
      <img
        className="movie-poster"
        src={movie.posterUrl || fallbackPoster}
        alt={`${movie.title} poster`}
      />

      <div>
        <h2 className="card-title">{movie.title}</h2>
        <p className="muted-text">
          {movie.year || 'Unknown year'} · {movie.type || 'Movie'}
        </p>
      </div>

      <div className="card-actions">
        <select
          className="select-input"
          value={category}
          onChange={(event) => setCategory(event.target.value)}
        >
          <option value="Watchlist">Watchlist</option>
          <option value="Favorite">Favorite</option>
        </select>
        <button
          type="button"
          className="primary-button"
          onClick={handleAdd}
          disabled={isSubmitting}
        >
          {isSubmitting ? 'Saving...' : 'Add'}
        </button>
      </div>
    </article>
  )
}

export default MovieCard
