import TaskCard from './TaskCard';

function TaskList({ tasks, loading, error, activeTaskId, onComplete, onDelete }) {
  return (
    <section className="rounded-2xl bg-slate-50 p-6 shadow-inner">
      <div className="mb-6 flex items-center justify-between gap-4">
        <div>
          <h2 className="text-2xl font-semibold text-slate-800">Tasks</h2>
          <p className="mt-2 text-sm text-slate-500">Your weather-related tasks are listed here.</p>
        </div>
        <span className="rounded-full bg-slate-200 px-3 py-1 text-sm font-medium text-slate-700">
          {tasks.length} items
        </span>
      </div>

      {loading ? <p className="text-sm text-slate-500">Loading tasks...</p> : null}

      {error ? (
        <div className="mb-4 rounded-xl border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
          {error}
        </div>
      ) : null}

      {!loading && tasks.length === 0 ? (
        <div className="rounded-2xl border border-dashed border-slate-300 bg-white px-6 py-8 text-center text-sm text-slate-500">
          No tasks yet. Search for a city first, then save a task.
        </div>
      ) : null}

      <div className="grid gap-4">
        {tasks.map((task) => (
          <TaskCard
            key={task.id}
            task={task}
            updating={activeTaskId?.id === task.id && activeTaskId?.type === 'update'}
            deleting={activeTaskId?.id === task.id && activeTaskId?.type === 'delete'}
            onComplete={onComplete}
            onDelete={onDelete}
          />
        ))}
      </div>
    </section>
  );
}

export default TaskList;
