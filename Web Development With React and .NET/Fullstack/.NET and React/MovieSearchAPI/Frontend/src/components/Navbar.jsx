import { NavLink } from 'react-router-dom'

function Navbar() {
  return (
    <header className="navbar">
      <div className="brand">Movie Search Planner</div>
      <nav className="nav-links">
        <NavLink
          to="/"
          className={({ isActive }) => `nav-link${isActive ? ' active' : ''}`}
        >
          Search
        </NavLink>
        <NavLink
          to="/tasks"
          className={({ isActive }) => `nav-link${isActive ? ' active' : ''}`}
        >
          Tasks
        </NavLink>
      </nav>
    </header>
  )
}

export default Navbar
