import { useEffect, useMemo, useState } from 'react'

const STORAGE_KEY = 'portfolio-theme'

function readInitialTheme() {
	const fromStorage = localStorage.getItem(STORAGE_KEY)

	if (fromStorage === 'light' || fromStorage === 'dark') {
		return fromStorage
	}

	return window.matchMedia('(prefers-color-scheme: dark)').matches
		? 'dark'
		: 'light'
}

export function useTheme() {
	const [theme, setTheme] = useState(readInitialTheme)

	useEffect(() => {
		document.documentElement.setAttribute('data-theme', theme)
		localStorage.setItem(STORAGE_KEY, theme)
	}, [theme])

	const toggleTheme = useMemo(
		() => () => setTheme((current) => (current === 'light' ? 'dark' : 'light')),
		[],
	)

	return { theme, toggleTheme }
}
