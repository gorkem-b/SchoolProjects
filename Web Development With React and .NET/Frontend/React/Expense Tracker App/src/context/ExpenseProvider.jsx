import { useMemo } from 'react'
import { useLocalStorage } from '../hooks/useLocalStorage'
import { ExpenseContext } from './ExpenseContext'

function ExpenseProvider({ children }) {
  const [expenses, setExpenses] = useLocalStorage('expenses', [])

  const addExpense = ({ text, amount }) => {
    const parsedAmount = Number(amount)

    // Business rule: expenses are stored as negative numbers.
    if (!text?.trim() || Number.isNaN(parsedAmount) || parsedAmount >= 0) {
      return false
    }

    const newExpense = {
      id: crypto.randomUUID(),
      text: text.trim(),
      amount: parsedAmount,
      createdAt: Date.now(),
    }

    setExpenses((prevExpenses) => [newExpense, ...prevExpenses])
    return true
  }

  const deleteExpense = (id) => {
    setExpenses((prevExpenses) => prevExpenses.filter((expense) => expense.id !== id))
  }

  // Derived totals are memoized to avoid recalculating on unrelated renders.
  const income = useMemo(
    () => expenses.filter((item) => item.amount > 0).reduce((sum, item) => sum + item.amount, 0),
    [expenses],
  )

  const expense = useMemo(
    () => expenses.filter((item) => item.amount < 0).reduce((sum, item) => sum + item.amount, 0),
    [expenses],
  )

  const balance = useMemo(() => income + expense, [income, expense])

  const value = {
    expenses,
    addExpense,
    deleteExpense,
    balance,
    income,
    expense,
  }

  return <ExpenseContext.Provider value={value}>{children}</ExpenseContext.Provider>
}

export default ExpenseProvider
