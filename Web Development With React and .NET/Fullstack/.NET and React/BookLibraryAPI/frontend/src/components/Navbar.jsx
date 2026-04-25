import { NavLink } from 'react-router-dom'

function getLinkClassName({ isActive }) {
  return isActive ? 'nav__link nav__link--active' : 'nav__link'
}

function Navbar() {
  return (
    <header className="app-header">
      <div className="app-header__content">
        <NavLink className="brand" to="/">
          <span className="brand__title">Book Library</span>
          <span className="brand__subtitle">Books and reading tasks in one place</span>
        </NavLink>

        <nav className="nav" aria-label="Main navigation">
          <NavLink className={getLinkClassName} to="/">
            Home
          </NavLink>
          <NavLink className={getLinkClassName} to="/books">
            Books
          </NavLink>
          <NavLink className={getLinkClassName} to="/tasks">
            Tasks
          </NavLink>
        </nav>
      </div>
    </header>
  )
}

export default Navbar