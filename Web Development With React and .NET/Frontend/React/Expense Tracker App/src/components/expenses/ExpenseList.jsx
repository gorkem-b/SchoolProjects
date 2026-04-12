import { useExpense } from '../../context/useExpense'
import ExpenseItem from './ExpenseItem'

function ExpenseList() {
  const { expenses, deleteExpense } = useExpense()

  return (
    <section className="card expense-list-card" aria-label="Expense list">
      <h2>Transactions</h2>
      {expenses.length === 0 ? (
        <p className="empty-state">Harcama yok</p>
      ) : (
        <ul className="expense-list">
          {expenses.map((expense) => (
            <ExpenseItem key={expense.id} expense={expense} onDelete={deleteExpense} />
          ))}
        </ul>
      )}
    </section>
  )
}

export default ExpenseList
