import { useEffect, useState } from 'react'

export function useLocalStorage(key, initialValue) {
  // Lazy initializer prevents reading localStorage on every render.
  const [storedValue, setStoredValue] = useState(() => {
    try {
      const item = window.localStorage.getItem(key)
      return item ? JSON.parse(item) : initialValue
    } catch {
      return initialValue
    }
  })

  useEffect(() => {
    try {
      // Keep storage in sync whenever key or state changes.
      window.localStorage.setItem(key, JSON.stringify(storedValue))
    } catch {
      // Ignore write errors in environments with blocked storage.
    }
  }, [key, storedValue])

  return [storedValue, setStoredValue]
}
