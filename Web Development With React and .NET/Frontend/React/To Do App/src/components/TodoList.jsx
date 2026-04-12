import { useState } from 'react'
import TodoInput from './TodoInput'
import TodoItem from './TodoItem'

export default function TodoList() {
  const [todos, setTodos] = useState([])
  const [input, setInput] = useState('')

  const addTodo = () => {
    const trimmed = input.trim()
    if (trimmed) {
      setTodos((prev) => [
        ...prev,
        { id: Date.now(), text: trimmed, completed: false },
      ])
      setInput('')
    }
  }

  const toggleTodo = (id) => {
    setTodos((prev) =>
      prev.map((todo) =>
        todo.id === id ? { ...todo, completed: !todo.completed } : todo,
      ),
    )
  }

  const deleteTodo = (id) => {
    setTodos((prev) => prev.filter((todo) => todo.id !== id))
  }

  return (
    <section className="todo-card" aria-label="Todo list card">
      <TodoInput value={input} onChange={setInput} onAddTodo={addTodo} />

      {todos.length === 0 ? (
        <div className="empty-state">
          <div className="empty-state-icon" aria-hidden="true">
            [ ]
          </div>
          <p className="empty-state-text">No tasks yet. Add your first one.</p>
        </div>
      ) : (
        <div className="todo-list" role="list">
          {todos.map((todo) => (
            <TodoItem
              key={todo.id}
              id={todo.id}
              text={todo.text}
              completed={todo.completed}
              onToggle={toggleTodo}
              onDelete={deleteTodo}
            />
          ))}
        </div>
      )}
    </section>
  )
}
