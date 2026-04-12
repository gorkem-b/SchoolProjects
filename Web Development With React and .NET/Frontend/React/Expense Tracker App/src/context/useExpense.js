import { useContext } from 'react'
import { ExpenseContext } from './ExpenseContext'

export function useExpense() {
  const context = useContext(ExpenseContext)

  // Fail fast to make provider wiring issues obvious during development.
  if (!context) {
    throw new Error('useExpense must be used inside ExpenseProvider')
  }

  return context
}
