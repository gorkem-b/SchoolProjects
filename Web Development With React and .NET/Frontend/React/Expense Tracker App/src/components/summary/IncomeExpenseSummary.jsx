import { useExpense } from '../../context/useExpense'
import { formatCurrency } from '../../utils/formatters'

function IncomeExpenseSummary() {
  const { income, expense } = useExpense()

  return (
    <section className="summary-grid" aria-label="Income and expense summary">
      <article className="card summary-item">
        <h3>Income</h3>
        <p className="money positive">{formatCurrency(income)}</p>
      </article>
      <article className="card summary-item">
        <h3>Expense</h3>
        <p className="money negative">{formatCurrency(expense)}</p>
      </article>
    </section>
  )
}

export default IncomeExpenseSummary
