import { useEffect, useState } from 'react'

export function useFetch(url, options) {
	const [data, setData] = useState(null)
	const [isLoading, setIsLoading] = useState(false)
	const [error, setError] = useState(null)

	useEffect(() => {
		if (!url) {
			return
		}

		const controller = new AbortController()

		async function run() {
			setIsLoading(true)
			setError(null)

			try {
				const response = await fetch(url, {
					...options,
					signal: controller.signal,
				})

				if (!response.ok) {
					throw new Error(`Request failed with status ${response.status}`)
				}

				const json = await response.json()
				setData(json)
			} catch (fetchError) {
				if (fetchError.name !== 'AbortError') {
					setError(fetchError)
				}
			} finally {
				setIsLoading(false)
			}
		}

		run()

		return () => controller.abort()
	}, [url, options])

	return { data, isLoading, error }
}
