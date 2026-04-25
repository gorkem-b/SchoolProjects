import { useEffect, useState } from 'react';
import { toDateTimeLocalValue } from '../utils/formatDate.js';

const emptyForm = {
  title: '',
  description: '',
  dueDate: '',
  category: 'General',
  status: 'Pending',
  isNotified: false,
};

export default function TaskForm({ task, onSubmit, onCancel }) {
  const [form, setForm] = useState(emptyForm);

  useEffect(() => {
    if (!task) {
      setForm(emptyForm);
      return;
    }

    setForm({
      title: task.title || '',
      description: task.description || '',
      dueDate: toDateTimeLocalValue(task.dueDate),
      category: task.category || 'General',
      status: task.status || 'Pending',
      isNotified: Boolean(task.isNotified),
    });
  }, [task]);

  const handleChange = (event) => {
    const { name, value, type, checked } = event.target;
    setForm((current) => ({
      ...current,
      [name]: type === 'checkbox' ? checked : value,
    }));
  };

  const handleSubmit = async (event) => {
    event.preventDefault();

    const payload = {
      title: form.title.trim(),
      description: form.description.trim(),
      dueDate: new Date(form.dueDate).toISOString(),
      category: form.category.trim() || 'General',
    };

    if (task) {
      payload.status = form.status;
      payload.isNotified = form.isNotified;
    }

    await onSubmit(payload);
    setForm(emptyForm);
  };

  return (
    <form onSubmit={handleSubmit} className="rounded-xl border bg-white p-4 shadow-sm">
      <div className="mb-4">
        <h2 className="text-lg font-semibold">{task ? 'Update Task' : 'Add New Task'}</h2>
        <p className="text-sm text-slate-500">Enter a title, description, date/time, and category.</p>
      </div>

      <div className="grid gap-4 md:grid-cols-2">
        <label className="block">
          <span className="text-sm font-medium text-slate-700">Title</span>
          <input
            name="title"
            value={form.title}
            onChange={handleChange}
            required
            maxLength={120}
            className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
          />
        </label>

        <label className="block">
          <span className="text-sm font-medium text-slate-700">Category</span>
          <input
            name="category"
            value={form.category}
            onChange={handleChange}
            maxLength={80}
            className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
          />
        </label>

        <label className="block md:col-span-2">
          <span className="text-sm font-medium text-slate-700">Description</span>
          <textarea
            name="description"
            value={form.description}
            onChange={handleChange}
            maxLength={1000}
            rows={3}
            className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
          />
        </label>

        <label className="block">
          <span className="text-sm font-medium text-slate-700">Date and Time</span>
          <input
            name="dueDate"
            type="datetime-local"
            value={form.dueDate}
            onChange={handleChange}
            required
            className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
          />
        </label>

        {task && (
          <label className="block">
            <span className="text-sm font-medium text-slate-700">Status</span>
            <select
              name="status"
              value={form.status}
              onChange={handleChange}
              className="mt-1 w-full rounded-lg border px-3 py-2 outline-none focus:border-blue-500"
            >
              <option value="Pending">Pending</option>
              <option value="Completed">Completed</option>
              <option value="Notified">Notified</option>
            </select>
          </label>
        )}
      </div>

      {task && (
        <label className="mt-4 flex items-center gap-2 text-sm text-slate-700">
          <input name="isNotified" type="checkbox" checked={form.isNotified} onChange={handleChange} />
          Mark as notified
        </label>
      )}

      <div className="mt-4 flex gap-2">
        <button type="submit" className="rounded-lg bg-blue-600 px-4 py-2 font-medium text-white hover:bg-blue-700">
          {task ? 'Update' : 'Add'}
        </button>
        {task && (
          <button type="button" onClick={onCancel} className="rounded-lg border px-4 py-2 font-medium text-slate-700 hover:bg-slate-50">
            Cancel
          </button>
        )}
      </div>
    </form>
  );
}
