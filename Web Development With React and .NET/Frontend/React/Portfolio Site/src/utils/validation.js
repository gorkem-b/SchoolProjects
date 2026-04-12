export function isRequired(value) {
	return String(value ?? '').trim().length > 0
}

export function isValidEmail(value) {
	if (!isRequired(value)) {
		return false
	}

	return /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(value)
}
