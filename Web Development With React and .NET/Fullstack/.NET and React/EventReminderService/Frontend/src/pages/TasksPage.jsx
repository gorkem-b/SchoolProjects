import { useEffect, useMemo, useState } from 'react';
import TaskForm from '../components/TaskForm.jsx';
import TaskList from '../components/TaskList.jsx';
import { useTaskStore } from '../store/useTaskStore.js';

const initialFilters = {
  category: '',
  status: '',
  isNotified: '',
};

export default function TasksPage() {
  const { tasks, fetchTasks, createTask, updateTask, deleteTask, isLoading, error } = useTaskStore();
  const [filters, setFilters] = useState(initialFilters);
  const [editingTask, setEditingTask] = useState(null);

  const apiFilters = useMemo(
    () => ({
      category: filters.category,
      status: filters.status,
      isNotified: filters.isNotified === '' ? undefined : filters.isNotified === 'true',
    }),
    [filters],
  );

  useEffect(() => {
    fetchTasks(apiFilters);
  }, [fetchTasks, apiFilters]);

  const handleFilterChange = (event) => {
    const { name, value } = event.target;
    setFilters((current) => ({ ...current, [name]: value }));
  };

  const handleSubmit = async (payload) => {
    if (editingTask) {
      await updateTask(editingTask.id, payload);
      setEditingTask(null);
    } else {
      await createTask(payload);
    }

    await fetchTasks(apiFilters);
  };

  const handleDelete = async (id) => {
    if (!window.confirm('Are you sure you want to delete this task?')) {
      return;
    }

    await deleteTask(id);
  };

  return (
    <div className="grid gap-6 lg:grid-cols-[380px_1fr]">
      <div className="space-y-4">
        <TaskForm task={editingTask} onSubmit={handleSubmit} onCancel={() => setEditingTask(null)} />

        <section className="rounded-xl border bg-white p-4 shadow-sm">
          <h2 className="mb-3 text-lg font-semibold">Filters</h2>
          <div className="space-y-3">
            <label className="block">
              <span className="text-sm font-medium text-slate-700">Category</span>
              <input
                name="category"
                value={filters.category}
                onChange={handleFilterChange}
                placeholder="Example: Work"
                className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
              />
            </label>

            <label className="block">
              <span className="text-sm font-medium text-slate-700">Status</span>
              <select
                name="status"
                value={filters.status}
                onChange={handleFilterChange}
                className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
              >
                <option value="">All</option>
                <option value="Pending">Pending</option>
                <option value="Completed">Completed</option>
                <option value="Notified">Notified</option>
              </select>
            </label>

            <label className="block">
              <span className="text-sm font-medium text-slate-700">Notification</span>
              <select
                name="isNotified"
                value={filters.isNotified}
                onChange={handleFilterChange}
                className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
              >
                <option value="">All</option>
                <option value="false">Not notified</option>
                <option value="true">Notified</option>
              </select>
            </label>

            <button
              type="button"
              onClick={() => setFilters(initialFilters)}
              className="w-full rounded-lg border px-4 py-2 font-medium text-slate-700 hover:bg-slate-50"
            >
              Clear Filters
            </button>
          </div>
        </section>
      </div>

      <section className="space-y-4">
        <div className="flex items-center justify-between">
          <div>
            <h2 className="text-2xl font-bold">Tasks</h2>
            <p className="text-sm text-slate-500">CRUD operations are synchronized with MSSQL through the API.</p>
          </div>
          {isLoading && <span className="text-sm text-slate-500">Loading...</span>}
        </div>

        {error && <div className="rounded-lg border border-red-200 bg-red-50 p-3 text-red-700">{error}</div>}

        <TaskList tasks={tasks} onEdit={setEditingTask} onDelete={handleDelete} />
      </section>
    </div>
  );
}
