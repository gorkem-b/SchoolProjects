export default function TodoInput({ value, onChange, onAddTodo }) {
  const handleSubmit = (event) => {
    event.preventDefault()
    onAddTodo()
  }

  return (
    <form onSubmit={handleSubmit} className="input-group">
      <input
        type="text"
        value={value}
        onChange={(event) => onChange(event.target.value)}
        placeholder="Add a new task..."
        className="todo-input"
        aria-label="New task"
      />
      <button type="submit" className="add-btn">
        Add
      </button>
    </form>
  )
}
