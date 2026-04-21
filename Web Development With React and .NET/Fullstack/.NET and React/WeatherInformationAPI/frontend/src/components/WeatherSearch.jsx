import { useState } from 'react';

function WeatherSearch({ weather, loading, error, onSearch }) {
  const [city, setCity] = useState('Istanbul');

  const handleSubmit = (event) => {
    event.preventDefault();
    onSearch(city);
  };

  return (
    <section className="rounded-2xl bg-white p-6 shadow-lg">
      <div className="mb-6">
        <h2 className="text-2xl font-semibold text-slate-800">Search weather</h2>
        <p className="mt-2 text-sm text-slate-500">
          Enter a city, review the current conditions, and save the result as a task.
        </p>
      </div>

      <form className="flex flex-col gap-3 sm:flex-row" onSubmit={handleSubmit}>
        <input
          type="text"
          value={city}
          onChange={(event) => setCity(event.target.value)}
          className="flex-1 rounded-xl border border-slate-300 px-4 py-3 text-slate-700 outline-none transition focus:border-sky-500"
          placeholder="e.g. London"
        />
        <button
          type="submit"
          disabled={loading}
          className="rounded-xl bg-sky-600 px-5 py-3 font-medium text-white transition hover:bg-sky-700 disabled:cursor-not-allowed disabled:bg-sky-300"
        >
          {loading ? 'Loading...' : 'Search'}
        </button>
      </form>

      {error ? (
        <div className="mt-4 rounded-xl border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
          {error}
        </div>
      ) : null}

      {weather ? (
        <div className="mt-6 rounded-2xl bg-slate-900 p-5 text-white">
          <div className="flex flex-col gap-4 sm:flex-row sm:items-center sm:justify-between">
            <div>
              <p className="text-sm uppercase tracking-wide text-slate-300">{weather.country}</p>
              <h3 className="text-3xl font-bold">{weather.cityName}</h3>
              <p className="mt-2 text-slate-300">{weather.description}</p>
            </div>

            <div className="flex items-center gap-3">
              <span className="text-6xl" aria-hidden="true">{weather.icon}</span>
              <div>
                <p className="text-4xl font-bold">{Math.round(weather.temperature)}°C</p>
                <p className="text-sm text-slate-300">Feels like: {Math.round(weather.feelsLike)}°C</p>
              </div>
            </div>
          </div>

          <div className="mt-5 grid gap-3 sm:grid-cols-2 lg:grid-cols-4">
            <WeatherMetric label="Humidity" value={`%${weather.humidity}`} />
            <WeatherMetric label="Wind" value={`${weather.windSpeed} km/h`} />
            <WeatherMetric label="Condition" value={weather.description} />
            <WeatherMetric label="City" value={weather.cityName} />
          </div>
        </div>
      ) : null}
    </section>
  );
}

function WeatherMetric({ label, value }) {
  return (
    <div className="rounded-xl bg-slate-800 px-4 py-3">
      <p className="text-xs uppercase tracking-wide text-slate-400">{label}</p>
      <p className="mt-1 text-sm font-medium text-white">{value}</p>
    </div>
  );
}

export default WeatherSearch;
