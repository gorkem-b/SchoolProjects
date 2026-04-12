import { useState } from "react";

export default function App() {
    const [task, setTask] = useState("");
    const [todos, setTodos] = useState([]);

    const addTodo = (e) => {
        e.preventDefault();
        const text = task.trim();
        if (!text) return;

        setTodos((prev) => [...prev, { id: Date.now(), text, done: false }]);
        setTask("");
    };

    const toggleTodo = (id) => {
        setTodos((prev) =>
            prev.map((todo) =>
                todo.id === id ? { ...todo, done: !todo.done } : todo
            )
        );
    };

    const removeTodo = (id) => {
        setTodos((prev) => prev.filter((todo) => todo.id !== id));
    };

    return (
        <main style={{ maxWidth: 520, margin: "40px auto", fontFamily: "Arial, sans-serif", padding: "0 16px" }}>
            <h1>To Do App</h1>

            <form onSubmit={addTodo} style={{ display: "flex", gap: 8, marginBottom: 16 }}>
                <input
                    value={task}
                    onChange={(e) => setTask(e.target.value)}
                    placeholder="Add a task..."
                    style={{ flex: 1, padding: "10px 12px" }}
                />
                <button type="submit" style={{ padding: "10px 14px" }}>
                    Add
                </button>
            </form>

            <ul style={{ listStyle: "none", padding: 0, margin: 0 }}>
                {todos.map((todo) => (
                    <li
                        key={todo.id}
                        style={{
                            display: "flex",
                            alignItems: "center",
                            justifyContent: "space-between",
                            padding: "10px 12px",
                            border: "1px solid #ddd",
                            borderRadius: 8,
                            marginBottom: 8,
                        }}
                    >
                        <label style={{ display: "flex", alignItems: "center", gap: 8, flex: 1 }}>
                            <input
                                type="checkbox"
                                checked={todo.done}
                                onChange={() => toggleTodo(todo.id)}
                            />
                            <span style={{ textDecoration: todo.done ? "line-through" : "none" }}>
                                {todo.text}
                            </span>
                        </label>
                        <button onClick={() => removeTodo(todo.id)} style={{ marginLeft: 10 }}>
                            Delete
                        </button>
                    </li>
                ))}
            </ul>
        </main>
    );
}
