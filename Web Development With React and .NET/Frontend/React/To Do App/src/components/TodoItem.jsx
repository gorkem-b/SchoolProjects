const TodoItem = ({ id, text, completed, onToggle, onDelete }) => {
  return (
    <div className={`todo-item ${completed ? 'completed' : ''}`}>
      <input
        type="checkbox"
        checked={completed}
        onChange={() => onToggle(id)}
        className="todo-checkbox"
      />
      <span className="todo-text">{text}</span>
        <button onClick={() => onDelete(id)} className="delete-btn">
          Delete
        </button>
    </div>
  )
}

export default TodoItem
