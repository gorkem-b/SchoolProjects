import { useEffect, useState } from 'react';
import WeatherSearch from './components/WeatherSearch';
import TaskForm from './components/TaskForm';
import TaskList from './components/TaskList';
import api from './services/api';
import useTaskStore from './store/taskStore';

function App() {
  const tasks = useTaskStore((state) => state.tasks);
  const tasksLoading = useTaskStore((state) => state.loading);
  const taskError = useTaskStore((state) => state.error);
  const fetchTasks = useTaskStore((state) => state.fetchTasks);
  const createTask = useTaskStore((state) => state.createTask);
  const updateTaskStatus = useTaskStore((state) => state.updateTaskStatus);
  const deleteTask = useTaskStore((state) => state.deleteTask);

  const [weather, setWeather] = useState(null);
  const [weatherLoading, setWeatherLoading] = useState(false);
  const [weatherError, setWeatherError] = useState(null);
  const [activeTaskId, setActiveTaskId] = useState(null);
  const [creatingTask, setCreatingTask] = useState(false);

  useEffect(() => {
    fetchTasks();
  }, [fetchTasks]);

  const handleWeatherSearch = async (city) => {
    const trimmedCity = city.trim();

    if (!trimmedCity) {
      setWeather(null);
      setWeatherError('Please enter a city.');
      return;
    }

    setWeatherLoading(true);
    setWeatherError(null);

    try {
      const response = await api.get(`/weather/${encodeURIComponent(trimmedCity)}`);
      setWeather(response.data);
    } catch (error) {
      setWeather(null);
      setWeatherError(
        error.response?.data?.message ||
          (error.request ? 'Could not reach the API.' : 'Could not fetch weather data.')
      );
    } finally {
      setWeatherLoading(false);
    }
  };

  const handleCreateTask = async (taskData) => {
    setCreatingTask(true);

    try {
      await createTask(taskData);
      return true;
    } catch {
      return false;
    } finally {
      setCreatingTask(false);
    }
  };

  const handleCompleteTask = async (task) => {
    setActiveTaskId({ id: task.id, type: 'update' });

    try {
      await updateTaskStatus(task, 'Completed');
    } catch {
      return;
    } finally {
      setActiveTaskId(null);
    }
  };

  const handleDeleteTask = async (taskId) => {
    setActiveTaskId({ id: taskId, type: 'delete' });

    try {
      await deleteTask(taskId);
    } catch {
      return;
    } finally {
      setActiveTaskId(null);
    }
  };

  return (
    <div className="min-h-screen bg-slate-200">
      <div className="mx-auto flex min-h-screen w-full max-w-7xl flex-col gap-8 px-4 py-10 sm:px-6 lg:px-8">
        <header className="rounded-3xl bg-slate-900 px-8 py-10 text-white shadow-xl">
          <p className="text-sm uppercase tracking-[0.3em] text-sky-300">Weather Information API</p>
          <h1 className="mt-4 text-4xl font-bold">Weather-driven task planning</h1>
          <p className="mt-4 max-w-3xl text-sm text-slate-300 sm:text-base">
            This app uses React, Zustand, .NET 10, and MSSQL to search weather by city, cache the
            result, and turn it into actionable tasks.
          </p>
        </header>

        <div className="grid gap-8 xl:grid-cols-[1.2fr_0.8fr]">
          <div className="grid gap-8">
            <WeatherSearch
              weather={weather}
              loading={weatherLoading}
              error={weatherError}
              onSearch={handleWeatherSearch}
            />

            <TaskList
              tasks={tasks}
              loading={tasksLoading}
              error={taskError}
              activeTaskId={activeTaskId}
              onComplete={handleCompleteTask}
              onDelete={handleDeleteTask}
            />
          </div>

          <div>
            {weather ? (
              <TaskForm cityName={weather.cityName} submitting={creatingTask} onSubmit={handleCreateTask} />
            ) : (
              <div className="rounded-2xl border border-dashed border-slate-300 bg-white px-6 py-10 text-center text-sm text-slate-500 shadow-sm">
                Search for a city first. The task form for the selected city will appear here.
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
