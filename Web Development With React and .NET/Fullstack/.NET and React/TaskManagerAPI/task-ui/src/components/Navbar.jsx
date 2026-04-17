import { Link } from "react-router-dom";
import { useAuth } from "../context/AuthContext";

export default function Navbar() {
  const { isAuthenticated, logout } = useAuth();

  return (
    <header className="navbar">
      <Link to="/">
        <strong>Task Manager</strong>
      </Link>

      <nav className="navbar-links">
        {isAuthenticated ? (
          <>
            <Link to="/">Dashboard</Link>
            <button className="button button-secondary" type="button" onClick={logout}>
              Logout
            </button>
          </>
        ) : (
          <>
            <Link to="/login">Login</Link>
            <Link to="/register">Register</Link>
          </>
        )}
      </nav>
    </header>
  );
}