import { useEffect, useMemo, useState } from 'react'
import axiosClient from '../api/axiosClient'
import TaskItem from '../components/TaskItem'

const filters = [
  { key: 'all', label: 'All' },
  { key: 'pending', label: 'Pending' },
  { key: 'watched', label: 'Watched' },
]

function TasksPage() {
  const [tasks, setTasks] = useState([])
  const [filter, setFilter] = useState('all')
  const [isLoading, setIsLoading] = useState(true)
  const [busyTaskId, setBusyTaskId] = useState(null)
  const [error, setError] = useState('')

  const filteredTasks = useMemo(() => {
    if (filter === 'pending') {
      return tasks.filter((task) => !task.isWatched)
    }

    if (filter === 'watched') {
      return tasks.filter((task) => task.isWatched)
    }

    return tasks
  }, [filter, tasks])

  useEffect(() => {
    let isCancelled = false

    axiosClient
      .get('/tasks')
      .then((response) => {
        if (isCancelled) {
          return
        }

        setTasks(response.data)
      })
      .catch(() => {
        if (isCancelled) {
          return
        }

        setError('Task list could not be loaded.')
      })
      .finally(() => {
        if (isCancelled) {
          return
        }

        setIsLoading(false)
      })

    return () => {
      isCancelled = true
    }
  }, [])

  const handleUpdate = async (taskId, updatedTask) => {
    setBusyTaskId(taskId)
    setError('')

    try {
      const response = await axiosClient.put(`/tasks/${taskId}`, updatedTask)
      setTasks((currentTasks) =>
        currentTasks.map((task) => (task.id === taskId ? response.data : task)),
      )
    } catch {
      setError('Task update failed.')
    } finally {
      setBusyTaskId(null)
    }
  }

  const handleDelete = async (taskId) => {
    setBusyTaskId(taskId)
    setError('')

    try {
      await axiosClient.delete(`/tasks/${taskId}`)
      setTasks((currentTasks) => currentTasks.filter((task) => task.id !== taskId))
    } catch {
      setError('Task delete failed.')
    } finally {
      setBusyTaskId(null)
    }
  }

  return (
    <section>
      <header className="page-header">
        <h1 className="page-title">Your tasks</h1>
        <p className="page-description">
          Review saved movies, filter watched items, update categories, and remove tasks you
          no longer need.
        </p>
      </header>

      <div className="filter-row">
        {filters.map((item) => (
          <button
            key={item.key}
            type="button"
            className={`filter-chip${filter === item.key ? ' active' : ''}`}
            onClick={() => setFilter(item.key)}
          >
            {item.label}
          </button>
        ))}
      </div>

      {error ? <p className="status-message error">{error}</p> : null}

      {isLoading ? (
        <div className="empty-state">Loading tasks...</div>
      ) : filteredTasks.length > 0 ? (
        <div className="grid">
          {filteredTasks.map((task) => (
            <TaskItem
              key={task.id}
              task={task}
              onUpdate={handleUpdate}
              onDelete={handleDelete}
              isBusy={busyTaskId === task.id}
            />
          ))}
        </div>
      ) : (
        <div className="empty-state">No tasks match the selected filter.</div>
      )}
    </section>
  )
}

export default TasksPage
