import { createContext, useContext, useMemo, useState } from "react";

const AuthContext = createContext(null);
const tokenStorageKey = "token";

export function AuthProvider({ children }) {
  const [token, setToken] = useState(() => localStorage.getItem(tokenStorageKey));

  const loginUser = (jwt) => {
    localStorage.setItem(tokenStorageKey, jwt);
    setToken(jwt);
  };

  const logout = () => {
    localStorage.removeItem(tokenStorageKey);
    setToken(null);
  };

  const value = useMemo(
    () => ({
      token,
      isAuthenticated: Boolean(token),
      loginUser,
      logout,
    }),
    [token]
  );

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>;
}

export function useAuth() {
  const context = useContext(AuthContext);

  if (!context) {
    throw new Error("useAuth must be used within an AuthProvider");
  }

  return context;
}