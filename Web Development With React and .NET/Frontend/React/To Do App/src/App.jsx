import TodoList from './components/TodoList'

function App() {
  return (
    <main className="app-container">
      <header className="app-header">
        <h1 className="app-title">To Do App</h1>
        <p className="app-subtitle">Track your tasks and stay focused</p>
      </header>

      <TodoList />
    </main>
  )
}

export default App
