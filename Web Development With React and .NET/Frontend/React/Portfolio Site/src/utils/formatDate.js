export function formatDate(input, locale = 'tr-TR') {
	const date = new Date(input)

	if (Number.isNaN(date.getTime())) {
		return ''
	}

	return new Intl.DateTimeFormat(locale, {
		year: 'numeric',
		month: 'short',
	}).format(date)
}
