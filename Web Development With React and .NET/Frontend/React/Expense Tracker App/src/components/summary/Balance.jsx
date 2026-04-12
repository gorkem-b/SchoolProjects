import { useExpense } from '../../context/useExpense'
import { formatCurrency } from '../../utils/formatters'

function Balance() {
  const { balance } = useExpense()
  const statusClass = balance < 0 ? 'negative' : 'positive'

  return (
    <section className="card balance-card" aria-label="Current balance">
      <h2>Current Balance</h2>
      <p className={`balance-value ${statusClass}`}>{formatCurrency(balance)}</p>
    </section>
  )
}

export default Balance
