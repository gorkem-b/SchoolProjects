import { useState } from 'react'
import { useExpense } from '../../context/useExpense'

function ExpenseForm() {
  const { addExpense } = useExpense()
  const [text, setText] = useState('')
  const [amount, setAmount] = useState('')
  const [error, setError] = useState('')

  const handleSubmit = (event) => {
    event.preventDefault()

    // Keep invalid payloads out of global state.
    if (!text.trim()) {
      setError('Description is required.')
      return
    }

    const parsedAmount = Number(amount)
    if (Number.isNaN(parsedAmount) || parsedAmount <= 0) {
      setError('Amount must be a positive number.')
      return
    }

    // User enters a positive amount; provider stores expenses as negative values.
    const didAdd = addExpense({ text, amount: parsedAmount * -1 })

    if (!didAdd) {
      setError('Expense could not be saved.')
      return
    }

    setText('')
    setAmount('')
    setError('')
  }

  return (
    <section className="card form-card" aria-label="Add expense form">
      <h2>Add Expense</h2>
      <form onSubmit={handleSubmit} className="expense-form">
        <label htmlFor="expense-text">Description</label>
        <input
          id="expense-text"
          type="text"
          value={text}
          onChange={(event) => setText(event.target.value)}
          placeholder="Groceries"
        />

        <label htmlFor="expense-amount">Amount</label>
        <input
          id="expense-amount"
          type="text"
          inputMode="decimal"
          autoComplete="off"
          value={amount}
          onChange={(event) => setAmount(event.target.value)}
          placeholder="500"
        />

        {error && <p className="form-error">{error}</p>}

        <button type="submit">Save Expense</button>
      </form>
    </section>
  )
}

export default ExpenseForm
