import { useEffect, useMemo, useState } from 'react'
import { getBooks } from '../api/bookApi'
import { createTask, deleteTask, getTasks, updateTask } from '../api/taskApi'
import EmptyState from '../components/EmptyState'
import TaskForm from '../components/TaskForm'
import TaskItem from '../components/TaskItem'
import { taskStatusOptions } from '../constants/taskStatuses'

function getErrorMessage(error) {
  return error?.response?.data || 'An error occurred while processing the request.'
}

function TasksPage() {
  const [books, setBooks] = useState([])
  const [tasks, setTasks] = useState([])
  const [loading, setLoading] = useState(true)
  const [submitting, setSubmitting] = useState(false)
  const [savingId, setSavingId] = useState(null)
  const [deletingId, setDeletingId] = useState(null)
  const [filter, setFilter] = useState('All')
  const [errorMessage, setErrorMessage] = useState('')

  useEffect(() => {
    let isMounted = true

    Promise.all([getBooks(), getTasks()])
      .then(([bookData, taskData]) => {
        if (isMounted) {
          setBooks(bookData)
          setTasks(taskData)
        }
      })
      .catch((error) => {
        if (isMounted) {
          setErrorMessage(getErrorMessage(error))
        }
      })
      .finally(() => {
        if (isMounted) {
          setLoading(false)
        }
      })

    return () => {
      isMounted = false
    }
  }, [])

  const filteredTasks = useMemo(() => {
    if (filter === 'All') {
      return tasks
    }

    return tasks.filter((task) => task.status === filter)
  }, [filter, tasks])

  async function handleCreateTask(formData) {
    setSubmitting(true)
    setErrorMessage('')

    try {
      const createdTask = await createTask(formData)
      setTasks((current) => [createdTask, ...current])
      return true
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
      return false
    } finally {
      setSubmitting(false)
    }
  }

  async function handleSaveTask(id, formData) {
    setSavingId(id)
    setErrorMessage('')

    try {
      await updateTask(id, formData)
      setTasks((current) =>
        current.map((task) => (task.id === id ? { ...task, ...formData } : task)),
      )
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    } finally {
      setSavingId(null)
    }
  }

  async function handleDeleteTask(id) {
    setDeletingId(id)
    setErrorMessage('')

    try {
      await deleteTask(id)
      setTasks((current) => current.filter((task) => task.id !== id))
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    } finally {
      setDeletingId(null)
    }
  }

  return (
    <section className="page">
      <div className="page__header">
        <h1 className="page__title">Reading tasks</h1>
        <p className="page__description">
          Create tasks, update their status, and track completed reading work.
        </p>
      </div>

      <div className="panel-grid">
        <aside className="panel">
          <div className="panel__header">
            <h2 className="panel__title">Create a new task</h2>
            <p className="panel__description">
              Link tasks to books to keep your reading plan organized.
            </p>
          </div>

          <TaskForm books={books} onSubmit={handleCreateTask} submitting={submitting} />

          {books.length === 0 ? (
            <p className="helper-text">Add a book before creating reading tasks.</p>
          ) : null}

          {errorMessage ? <p className="feedback feedback--error">{errorMessage}</p> : null}
        </aside>

        <div className="panel">
          <div className="panel__header">
            <h2 className="panel__title">Task list</h2>
            <p className="panel__description">Total tasks: {tasks.length}</p>
          </div>

          <div className="filter-row" role="group" aria-label="Task filters">
            <button
              className={filter === 'All' ? 'filter-button filter-button--active' : 'filter-button'}
              type="button"
              onClick={() => setFilter('All')}
            >
              All
            </button>

            {taskStatusOptions.map((option) => (
              <button
                key={option.value}
                className={
                  filter === option.value
                    ? 'filter-button filter-button--active'
                    : 'filter-button'
                }
                type="button"
                onClick={() => setFilter(option.value)}
              >
                {option.label}
              </button>
            ))}
          </div>

          {loading ? <p className="helper-text">Loading tasks...</p> : null}

          {!loading && filteredTasks.length === 0 ? (
            <EmptyState
              title="No tasks to show"
              text="No tasks match the selected filter. Choose another filter or add a new task."
            />
          ) : null}

          <div className="task-list">
            {filteredTasks.map((task) => (
              <TaskItem
                key={task.id}
                task={task}
                onSave={handleSaveTask}
                onDelete={handleDeleteTask}
                saving={savingId === task.id}
                deleting={deletingId === task.id}
              />
            ))}
          </div>
        </div>
      </div>
    </section>
  )
}

export default TasksPage