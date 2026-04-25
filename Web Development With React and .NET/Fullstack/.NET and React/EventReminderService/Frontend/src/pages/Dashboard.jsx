import { useEffect } from 'react';
import { Link } from 'react-router-dom';
import { useTaskStore } from '../store/useTaskStore.js';
import { formatDate } from '../utils/formatDate.js';

export default function Dashboard() {
  const { tasks, fetchTasks, isLoading, error } = useTaskStore();

  useEffect(() => {
    fetchTasks();
  }, [fetchTasks]);

  const now = new Date();
  const pendingTasks = tasks.filter((task) => task.status === 'Pending');
  const notifiedTasks = tasks.filter((task) => task.isNotified || task.status === 'Notified');
  const upcomingTasks = pendingTasks
    .filter((task) => new Date(task.dueDate) >= now)
    .sort((a, b) => new Date(a.dueDate) - new Date(b.dueDate))
    .slice(0, 5);

  return (
    <div className="space-y-6">
      <section className="rounded-xl border bg-white p-6 shadow-sm">
        <div className="flex flex-col gap-4 md:flex-row md:items-center md:justify-between">
          <div>
            <h2 className="text-2xl font-bold">Dashboard</h2>
            <p className="text-slate-500">Upcoming reminders and task summary.</p>
          </div>
          <Link to="/tasks" className="rounded-lg bg-blue-600 px-4 py-2 text-center font-medium text-white hover:bg-blue-700">
            Manage Tasks
          </Link>
        </div>
      </section>

      {error && <div className="rounded-lg border border-red-200 bg-red-50 p-3 text-red-700">{error}</div>}

      <section className="grid gap-4 md:grid-cols-3">
        <SummaryCard title="Total Tasks" value={tasks.length} />
        <SummaryCard title="Pending" value={pendingTasks.length} />
        <SummaryCard title="Notified" value={notifiedTasks.length} />
      </section>

      <section className="rounded-xl border bg-white p-6 shadow-sm">
        <h3 className="mb-4 text-lg font-semibold">Upcoming Tasks</h3>
        {isLoading ? (
          <p className="text-slate-500">Loading...</p>
        ) : upcomingTasks.length === 0 ? (
          <p className="text-slate-500">No upcoming pending tasks.</p>
        ) : (
          <div className="space-y-3">
            {upcomingTasks.map((task) => (
              <div key={task.id} className="rounded-lg border p-3">
                <div className="flex flex-col gap-1 md:flex-row md:items-center md:justify-between">
                  <div>
                    <p className="font-medium">{task.title}</p>
                    <p className="text-sm text-slate-500">{task.category}</p>
                  </div>
                  <p className="text-sm font-medium text-slate-600">{formatDate(task.dueDate)}</p>
                </div>
              </div>
            ))}
          </div>
        )}
      </section>
    </div>
  );
}

function SummaryCard({ title, value }) {
  return (
    <div className="rounded-xl border bg-white p-5 shadow-sm">
      <p className="text-sm font-medium text-slate-500">{title}</p>
      <p className="mt-2 text-3xl font-bold">{value}</p>
    </div>
  );
}
