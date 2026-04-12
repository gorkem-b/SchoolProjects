import { cleanup, render, screen, within } from '@testing-library/react'
import userEvent from '@testing-library/user-event'
import { readFileSync } from 'node:fs'
import { resolve } from 'node:path'
import process from 'node:process'
import { afterEach, beforeEach, describe, expect, it } from 'vitest'
import App from './App'

async function addExpense(description, amount) {
  const user = userEvent.setup()
  await user.type(screen.getByLabelText(/description/i), description)
  await user.type(screen.getByLabelText(/amount/i), amount)
  await user.click(screen.getByRole('button', { name: /save expense/i }))
}

describe('Manual checklist verification', () => {
  beforeEach(() => {
    window.localStorage.clear()
  })

  afterEach(() => {
    cleanup()
  })

  it('Core flow: adds expense at top and deletes correctly with empty state', async () => {
    render(<App />)

    await addExpense('Groceries', '100')
    await addExpense('Fuel', '50')

    const itemsBeforeDelete = screen.getAllByRole('listitem')
    expect(itemsBeforeDelete[0]).toHaveTextContent('Fuel')
    expect(itemsBeforeDelete[1]).toHaveTextContent('Groceries')

    const user = userEvent.setup()
    const deleteButtons = screen.getAllByRole('button', { name: /delete/i })
    await user.click(deleteButtons[0])
    await user.click(screen.getByRole('button', { name: /delete/i }))

    expect(screen.queryByRole('listitem')).not.toBeInTheDocument()
    expect(screen.getByText('Harcama yok')).toBeInTheDocument()
  })

  it('Validation: rejects empty, zero, negative, and non-numeric amount', async () => {
    render(<App />)

    const user = userEvent.setup()

    await user.click(screen.getByRole('button', { name: /save expense/i }))
    expect(screen.getByText(/description is required/i)).toBeInTheDocument()

    await user.type(screen.getByLabelText(/description/i), 'Rent')
    await user.type(screen.getByLabelText(/amount/i), '0')
    await user.click(screen.getByRole('button', { name: /save expense/i }))
    expect(screen.getByText(/amount must be a positive number/i)).toBeInTheDocument()

    await user.clear(screen.getByLabelText(/amount/i))
    await user.type(screen.getByLabelText(/amount/i), '-10')
    await user.click(screen.getByRole('button', { name: /save expense/i }))
    expect(screen.getByText(/amount must be a positive number/i)).toBeInTheDocument()

    await user.clear(screen.getByLabelText(/amount/i))
    await user.type(screen.getByLabelText(/amount/i), 'abc')
    await user.click(screen.getByRole('button', { name: /save expense/i }))
    expect(screen.getByText(/amount must be a positive number/i)).toBeInTheDocument()
  })

  it('Calculations: income stays zero, expense updates, balance decreases, and classes are correct', async () => {
    render(<App />)

    await addExpense('Coffee', '20')
    await addExpense('Taxi', '80')

    const incomeCard = screen.getByText('Income').closest('article')
    const expenseCard = screen.getByText('Expense').closest('article')
    const balanceCard = screen.getByLabelText('Current balance')

    const incomeText = within(incomeCard).getByText((content) => content.includes('₺0'))
    expect(incomeText).toBeInTheDocument()

    const expenseText = within(expenseCard).getByText((content) => content.includes('₺100'))
    expect(expenseText).toBeInTheDocument()

    const balanceText = within(balanceCard).getByText((content) => content.includes('₺100'))
    expect(balanceText).toHaveClass('negative')

    expect(expenseText).toHaveClass('negative')
  })

  it('Persistence: data survives remount and deletion is persisted', async () => {
    const firstRender = render(<App />)

    await addExpense('Internet', '30')
    await addExpense('Market', '40')

    firstRender.unmount()

    const secondRender = render(<App />)
    expect(screen.getByText('Market')).toBeInTheDocument()
    expect(screen.getByText('Internet')).toBeInTheDocument()

    const user = userEvent.setup()
    await user.click(screen.getAllByRole('button', { name: /delete/i })[0])

    secondRender.unmount()
    render(<App />)

    expect(screen.queryByText('Market')).not.toBeInTheDocument()
    expect(screen.getByText('Internet')).toBeInTheDocument()
  })

  it('Responsive/UI styles: desktop and mobile grid rules plus focus-visible selectors exist', () => {
    const css = readFileSync(resolve(process.cwd(), 'src/index.css'), 'utf8')

    expect(css).toContain('grid-template-columns: minmax(320px, 1fr) minmax(360px, 1fr);')
    expect(css).toContain('@media (max-width: 900px)')
    expect(css).toContain('grid-template-columns: 1fr;')
    expect(css).toContain('.expense-form input:focus-visible')
    expect(css).toContain('.expense-form button:focus-visible')
    expect(css).toContain('.expense-item button:focus-visible')
  })
})
