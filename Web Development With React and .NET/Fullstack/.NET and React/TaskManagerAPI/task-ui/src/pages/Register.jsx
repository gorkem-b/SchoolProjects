import { useEffect, useState } from "react";
import { Link, useNavigate } from "react-router-dom";
import { register } from "../api/authService";
import { useAuth } from "../context/AuthContext";

export default function Register() {
  const navigate = useNavigate();
  const { isAuthenticated } = useAuth();
  const [form, setForm] = useState({ username: "", password: "" });
  const [error, setError] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  useEffect(() => {
    if (isAuthenticated) {
      navigate("/", { replace: true });
    }
  }, [isAuthenticated, navigate]);

  const handleChange = (event) => {
    const { name, value } = event.target;
    setForm((current) => ({ ...current, [name]: value }));

    if (error) {
      setError("");
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
      const payload = {
        username: form.username.trim(),
        password: form.password,
      };

      const response = await register(payload);

      if (!response?.data || typeof response.data !== "object") {
        setError("Unexpected response format.");
        return;
      }

      navigate("/login", {
        replace: true,
        state: {
          message: "Account created successfully. Please sign in.",
        },
      });
    } catch (requestError) {
      if (!requestError.response) {
        setError("Network error: API unreachable.");
      } else if (requestError.response.status >= 500) {
        setError(`Server returned ${requestError.response.status}.`);
      } else if (requestError.response?.status === 409) {
        setError(requestError.response?.data?.message ?? "This username is already in use.");
      } else if (!requestError.response?.data || typeof requestError.response.data !== "object") {
        setError("Unexpected response format.");
      } else {
        setError(requestError.response?.data?.message ?? "Registration failed.");
      }
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <section className="card page-card">
      <h1>Register</h1>
      <p className="muted">Create a new account and sign in to the application right away.</p>

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
          autoComplete="new-password"
        />
        <button className="button" type="submit" disabled={isSubmitting || !form.username.trim() || !form.password.trim()}>
          {isSubmitting ? "Creating account..." : "Create account"}
        </button>
      </form>

      <p>
        Already have an account? <Link to="/login">Login</Link>
      </p>
    </section>
  );
}