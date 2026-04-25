function HomePage() {
  return (
    <section className="page">
      <div className="hero-panel">
        <div>
          <p className="hero-panel__eyebrow">Digital library management</p>
          <h1 className="hero-panel__title">Manage your books and reading tasks from one dashboard.</h1>
          <p className="hero-panel__description">
            This application combines a React frontend, a .NET API, and a SQL Server database
            to help you add books, create reading tasks, and track progress in one place.
          </p>

          <ul className="hero-panel__list">
            <li>Save books with title, author, and genre details.</li>
            <li>Create reading tasks for each book and update their status.</li>
            <li>Move between books and tasks to keep your reading plan organized.</li>
          </ul>
        </div>

        <div className="hero-panel__aside">
          <article className="stat-card">
            <p className="stat-card__value">3 Layers</p>
            <p className="stat-card__label">Frontend, backend, and MSSQL based structure</p>
          </article>

          <article className="stat-card">
            <p className="stat-card__value">CRUD</p>
            <p className="stat-card__label">Core management flow for book and task data</p>
          </article>

          <article className="stat-card">
            <p className="stat-card__value">Docker Ready</p>
            <p className="stat-card__label">Runs as a multi-service application with Compose</p>
          </article>
        </div>
      </div>
    </section>
  )
}

export default HomePage