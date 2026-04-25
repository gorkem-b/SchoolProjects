import { formatDate } from '../utils/formatDate.js';

const statusClass = {
  Pending: 'bg-amber-100 text-amber-800',
  Completed: 'bg-emerald-100 text-emerald-800',
  Notified: 'bg-blue-100 text-blue-800',
};

export default function TaskItem({ task, onEdit, onDelete }) {
  return (
    <article className="rounded-xl border bg-white p-4 shadow-sm">
      <div className="flex flex-col gap-3 md:flex-row md:items-start md:justify-between">
        <div>
          <div className="flex flex-wrap items-center gap-2">
            <h3 className="text-lg font-semibold">{task.title}</h3>
            <span className={`rounded-full px-2 py-1 text-xs font-semibold ${statusClass[task.status] || 'bg-slate-100 text-slate-700'}`}>
              {task.status}
            </span>
            {task.isNotified && <span className="rounded-full bg-slate-100 px-2 py-1 text-xs font-semibold text-slate-700">Notified</span>}
          </div>
          {task.description && <p className="mt-2 text-sm text-slate-600">{task.description}</p>}
          <dl className="mt-3 flex flex-wrap gap-x-6 gap-y-2 text-sm text-slate-500">
            <div>
              <dt className="font-medium text-slate-700">Date</dt>
              <dd>{formatDate(task.dueDate)}</dd>
            </div>
            <div>
              <dt className="font-medium text-slate-700">Category</dt>
              <dd>{task.category}</dd>
            </div>
          </dl>
        </div>
        <div className="flex gap-2">
          <button onClick={() => onEdit(task)} className="rounded-lg border px-3 py-2 text-sm font-medium text-slate-700 hover:bg-slate-50">
            Edit
          </button>
          <button onClick={() => onDelete(task.id)} className="rounded-lg bg-red-600 px-3 py-2 text-sm font-medium text-white hover:bg-red-700">
            Delete
          </button>
        </div>
      </div>
    </article>
  );
}
