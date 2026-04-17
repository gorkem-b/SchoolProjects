import { useCallback, useEffect, useMemo, useState } from "react";
import { createTask, deleteTask, getTasks, updateTask } from "../api/taskService";
import { useAuth } from "../context/AuthContext";
import TaskForm from "../components/TaskForm";
import TaskItem from "../components/TaskItem";

const statusOptions = ["All", "Pending", "InProgress", "Completed"];

export default function Dashboard() {
  const { logout } = useAuth();
  const [tasks, setTasks] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [isCreating, setIsCreating] = useState(false);
  const [isUpdating, setIsUpdating] = useState(false);
  const [deletingTaskId, setDeletingTaskId] = useState(null);
  const [editingTask, setEditingTask] = useState(null);
  const [error, setError] = useState("");
  const [successMessage, setSuccessMessage] = useState("");
  const [statusFilter, setStatusFilter] = useState("All");
  const [categoryFilter, setCategoryFilter] = useState("All");

  const loadTasks = useCallback(async () => {
    setIsLoading(true);
    setError("");

    try {
      const response = await getTasks();
      setTasks(response.data);
    } catch (requestError) {
      if (requestError.response?.status === 401) {
        logout();
        return;
      }

      setError(requestError.response?.data?.message ?? "Tasks could not be loaded.");
    } finally {
      setIsLoading(false);
    }
  }, [logout]);

  useEffect(() => {
    loadTasks();
  }, [loadTasks]);

  useEffect(() => {
    if (!successMessage) {
      return undefined;
    }

    const timeoutId = window.setTimeout(() => {
      setSuccessMessage("");
    }, 3000);

    return () => window.clearTimeout(timeoutId);
  }, [successMessage]);

  const availableCategories = useMemo(() => {
    const categories = Array.from(
      new Set(tasks.map((task) => task.category).filter(Boolean))
    ).sort((left, right) => left.localeCompare(right));

    return ["All", ...categories];
  }, [tasks]);

  const filteredTasks = useMemo(() => {
    return tasks.filter((task) => {
      const matchesStatus = statusFilter === "All" || task.status === statusFilter;
      const matchesCategory = categoryFilter === "All" || task.category === categoryFilter;

      return matchesStatus && matchesCategory;
    });
  }, [categoryFilter, statusFilter, tasks]);

  const handleCreateTask = async (payload) => {
    setIsCreating(true);
    setError("");
    setSuccessMessage("");

    try {
      const response = await createTask(payload);
      setTasks((current) => [response.data, ...current]);
      setSuccessMessage("Task created successfully.");
      return true;
    } catch (requestError) {
      if (requestError.response?.status === 401) {
        logout();
        return false;
      }

      setError(requestError.response?.data?.message ?? "Task could not be created.");
      return false;
    } finally {
      setIsCreating(false);
    }
  };

  const handleFormInteraction = () => {
    if (error) {
      setError("");
    }

    if (successMessage) {
      setSuccessMessage("");
    }
  };

  const handleStartEdit = (task) => {
    setEditingTask(task);
    setError("");
    setSuccessMessage("");
  };

  const handleCancelEdit = () => {
    setEditingTask(null);
    setSuccessMessage("");
  };

  const handleUpdateTask = async (payload) => {
    if (!editingTask) {
      return;
    }

    setIsUpdating(true);
    setError("");
    setSuccessMessage("");

    try {
      await updateTask(editingTask.id, payload);
      setTasks((current) =>
        current.map((task) =>
          task.id === editingTask.id
            ? {
                ...task,
                ...payload,
              }
            : task
        )
      );
      setEditingTask(null);
      setSuccessMessage("Task updated successfully.");
      return true;
    } catch (requestError) {
      if (requestError.response?.status === 401) {
        logout();
        return false;
      }

      setError(requestError.response?.data?.message ?? "Task could not be updated.");
      return false;
    } finally {
      setIsUpdating(false);
    }
  };

  const handleClearFilters = () => {
    setStatusFilter("All");
    setCategoryFilter("All");
    setSuccessMessage("");
  };

  const handleDeleteTask = async (taskId) => {
    const taskToDelete = tasks.find((task) => task.id === taskId);
    const isConfirmed = window.confirm(
      `Delete task${taskToDelete?.title ? ` \"${taskToDelete.title}\"` : ""}?`
    );

    if (!isConfirmed) {
      return;
    }

    setDeletingTaskId(taskId);
    setError("");
    setSuccessMessage("");

    try {
      await deleteTask(taskId);
      setTasks((current) => current.filter((task) => task.id !== taskId));
      if (editingTask?.id === taskId) {
        setEditingTask(null);
      }
      setSuccessMessage("Task deleted successfully.");
    } catch (requestError) {
      if (requestError.response?.status === 401) {
        logout();
        return;
      }

      setError(requestError.response?.data?.message ?? "Task could not be deleted.");
    } finally {
      setDeletingTaskId(null);
    }
  };

  return (
    <section className="dashboard-grid">
      <div className="card">
        <h1>Dashboard</h1>
        <p className="muted">View, filter, create, edit, and delete the tasks that belong to your account.</p>

        {error && <p className="feedback feedback-error">{error}</p>}
        {successMessage && <p className="feedback feedback-success">{successMessage}</p>}

        {editingTask && (
          <p className="feedback feedback-info">
            <strong>Editing:</strong> {editingTask.title}
          </p>
        )}
      </div>

      <TaskForm
        title={editingTask ? "Edit Task" : "Create Task"}
        initialValues={editingTask}
        onSubmit={editingTask ? handleUpdateTask : handleCreateTask}
        onInteraction={handleFormInteraction}
        submitLabel={
          editingTask
            ? isUpdating
              ? "Updating..."
              : "Update task"
            : isCreating
              ? "Saving..."
              : "Save task"
        }
        isSubmitting={isCreating || isUpdating}
        onCancel={handleCancelEdit}
        showCancel={Boolean(editingTask)}
      />

      <div className="card">
        <h2>Tasks</h2>

        <div className="filter-bar">
          <label className="filter-group">
            <span>Status</span>
            <select value={statusFilter} onChange={(event) => setStatusFilter(event.target.value)}>
              {statusOptions.map((status) => (
                <option key={status} value={status}>
                  {status}
                </option>
              ))}
            </select>
          </label>

          <label className="filter-group">
            <span>Category</span>
            <select value={categoryFilter} onChange={(event) => setCategoryFilter(event.target.value)}>
              {availableCategories.map((category) => (
                <option key={category} value={category}>
                  {category}
                </option>
              ))}
            </select>
          </label>

          <button
            className="button button-secondary filter-clear"
            type="button"
            onClick={handleClearFilters}
            disabled={statusFilter === "All" && categoryFilter === "All"}
          >
            Clear filters
          </button>
        </div>

        {!isLoading && tasks.length > 0 && (
          <p className="muted">
            Showing <strong>{filteredTasks.length}</strong> of <strong>{tasks.length}</strong> task(s).
          </p>
        )}

        {isLoading ? (
          <p className="muted">Loading tasks...</p>
        ) : tasks.length === 0 ? (
          <p className="muted">No tasks yet. Create your first task to get started.</p>
        ) : filteredTasks.length === 0 ? (
          <p className="muted">No tasks match the selected filters.</p>
        ) : (
          <div className="task-list">
            {filteredTasks.map((task) => (
              <TaskItem
                key={task.id}
                task={task}
                onEdit={handleStartEdit}
                onDelete={handleDeleteTask}
                isDeleting={deletingTaskId === task.id}
                isEditing={editingTask?.id === task.id}
              />
            ))}
          </div>
        )}
      </div>
    </section>
  );
}