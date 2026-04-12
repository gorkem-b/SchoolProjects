import Header from './components/layout/Header'
import Balance from './components/summary/Balance'
import IncomeExpenseSummary from './components/summary/IncomeExpenseSummary'
import ExpenseForm from './components/expenses/ExpenseForm'
import ExpenseList from './components/expenses/ExpenseList'
import ExpenseProvider from './context/ExpenseProvider'

function App() {
  return (
    <ExpenseProvider>
      <div className="app-shell">
        <Header />
        <main className="app-grid">
          <section className="left-column">
            <Balance />
            <IncomeExpenseSummary />
            <ExpenseForm />
          </section>
          <section className="right-column">
            <ExpenseList />
          </section>
        </main>
      </div>
    </ExpenseProvider>
  )
}

export default App
