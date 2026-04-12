export function formatCurrency(value, locale = 'tr-TR', currency = 'TRY') {
  // Fallback to 0 to avoid rendering NaN in the UI.
  return new Intl.NumberFormat(locale, {
    style: 'currency',
    currency,
    minimumFractionDigits: 2,
    maximumFractionDigits: 2,
  }).format(Number(value) || 0)
}
