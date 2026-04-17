import { useEffect, useState } from "react";
import { Link, useLocation, useNavigate } from "react-router-dom";
import { login } from "../api/authService";
import { useAuth } from "../context/AuthContext";

export default function Login() {
  const location = useLocation();
  const navigate = useNavigate();
  const { isAuthenticated, loginUser } = useAuth();
  const [form, setForm] = useState({ username: "", password: "" });
  const [error, setError] = useState("");
  const [successMessage, setSuccessMessage] = useState(location.state?.message ?? "");
  const [isSubmitting, setIsSubmitting] = useState(false);

  useEffect(() => {
    if (isAuthenticated) {
      navigate("/", { replace: true });
    }
  }, [isAuthenticated, navigate]);

  useEffect(() => {
    if (!location.state?.message) {
      return;
    }

    navigate(location.pathname, { replace: true, state: null });
  }, [location.pathname, location.state, navigate]);

  const handleChange = (event) => {
    const { name, value } = event.target;
    setForm((current) => ({ ...current, [name]: value }));

    if (error) {
      setError("");
    }

    if (successMessage) {
      setSuccessMessage("");
    }
  };

  const handleSubmit = async (event) => {
    event.preventDefault();

    if (!form.username.trim() || !form.password.trim()) {
      setError("Username and password are required.");
      return;
    }

    setIsSubmitting(true);
    setError("");

    try {
      const response = await login({
        username: form.username.trim(),
        password: form.password,
      });

      loginUser(response.data.token);
      navigate("/", { replace: true });
    } catch (requestError) {
      setError(requestError.response?.data?.message ?? "Login failed.");
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <section className="card page-card">
      <h1>Login</h1>
      <p className="muted">Sign in with your existing account to manage your tasks.</p>

      {successMessage && <p className="feedback feedback-success">{successMessage}</p>}
      {error && <p className="feedback feedback-error">{error}</p>}

      <form className="form-stack" onSubmit={handleSubmit}>
        <input
          name="username"
          placeholder="Username"
          value={form.username}
          onChange={handleChange}
          disabled={isSubmitting}
          autoComplete="username"
        />
        <input
          name="password"
          type="password"
          placeholder="Password"
          value={form.password}
          onChange={handleChange}
          disabled={isSubmitting}
          autoComplete="current-password"
        />
        <button className="button" type="submit" disabled={isSubmitting || !form.username.trim() || !form.password.trim()}>
          {isSubmitting ? "Logging in..." : "Login"}
        </button>
      </form>

      <p>
        Don't have an account? <Link to="/register">Register</Link>
      </p>
    </section>
  );
}