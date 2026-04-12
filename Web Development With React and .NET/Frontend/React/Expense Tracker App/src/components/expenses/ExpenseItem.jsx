import { formatCurrency } from '../../utils/formatters'

function ExpenseItem({ expense, onDelete }) {
  return (
    <li className="expense-item">
      <div>
        <p className="expense-title">{expense.text}</p>
        <small>{new Date(expense.createdAt).toLocaleString('tr-TR')}</small>
      </div>
      <div className="expense-actions">
        <strong>{formatCurrency(expense.amount)}</strong>
        <button type="button" onClick={() => onDelete(expense.id)}>
          Delete
        </button>
      </div>
    </li>
  )
}

export default ExpenseItem
