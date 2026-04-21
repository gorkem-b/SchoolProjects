function TaskCard({ task, onComplete, onDelete, updating, deleting }) {
  const isCompleted = task.status === 'Completed';

  return (
    <article className="rounded-2xl border border-slate-200 bg-white p-5 shadow-sm">
      <div className="flex flex-col gap-4 sm:flex-row sm:items-start sm:justify-between">
        <div>
          <div className="flex flex-wrap items-center gap-2">
            <h3 className="text-lg font-semibold text-slate-800">{task.title}</h3>
            <span
              className={`rounded-full px-3 py-1 text-xs font-semibold ${
                isCompleted
                  ? 'bg-emerald-100 text-emerald-700'
                  : 'bg-amber-100 text-amber-700'
              }`}
            >
              {task.status}
            </span>
          </div>

          <p className="mt-2 text-sm text-slate-500">
            {task.cityName} • {task.category}
          </p>

          {task.description ? <p className="mt-3 text-sm text-slate-700">{task.description}</p> : null}
        </div>

        <p className="text-xs text-slate-400">
          {new Date(task.createdAt).toLocaleString('en-US')}
        </p>
      </div>

      <div className="mt-5 flex flex-wrap gap-3">
        {!isCompleted ? (
          <button
            type="button"
            onClick={() => onComplete(task)}
            disabled={updating}
            className="rounded-xl bg-sky-600 px-4 py-2 text-sm font-medium text-white transition hover:bg-sky-700 disabled:cursor-not-allowed disabled:bg-sky-300"
          >
            {updating ? 'Updating...' : 'Mark as completed'}
          </button>
        ) : null}

        <button
          type="button"
          onClick={() => onDelete(task.id)}
          disabled={deleting}
          className="rounded-xl bg-rose-100 px-4 py-2 text-sm font-medium text-rose-700 transition hover:bg-rose-200 disabled:cursor-not-allowed disabled:opacity-60"
        >
          {deleting ? 'Deleting...' : 'Delete'}
        </button>
      </div>
    </article>
  );
}

export default TaskCard;
