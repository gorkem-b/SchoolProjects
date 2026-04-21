import { useEffect, useState } from 'react';

const initialForm = {
  title: '',
  description: '',
  category: ''
};

function TaskForm({ cityName, submitting, onSubmit }) {
  const [form, setForm] = useState(initialForm);

  useEffect(() => {
    setForm((current) => ({
      ...current,
      category: current.category || cityName
    }));
  }, [cityName]);

  const handleChange = (event) => {
    const { name, value } = event.target;
    setForm((current) => ({ ...current, [name]: value }));
  };

  const handleSubmit = async (event) => {
    event.preventDefault();

    const isSuccessful = await onSubmit({
      ...form,
      cityName
    });

    if (!isSuccessful) {
      return;
    }

    setForm((current) => ({
      ...initialForm,
      category: cityName
    }));
  };

  return (
    <section className="rounded-2xl bg-white p-6 shadow-lg">
      <div className="mb-6">
        <h2 className="text-2xl font-semibold text-slate-800">Create task</h2>
        <p className="mt-2 text-sm text-slate-500">
          Selected city: <span className="font-medium text-slate-700">{cityName}</span>
        </p>
      </div>

      <form className="grid gap-4" onSubmit={handleSubmit}>
        <label className="grid gap-2 text-sm font-medium text-slate-700">
          Title
          <input
            type="text"
            name="title"
            value={form.title}
            onChange={handleChange}
            required
            className="rounded-xl border border-slate-300 px-4 py-3 font-normal outline-none transition focus:border-sky-500"
            placeholder="e.g. Prepare for rainy weather"
          />
        </label>

        <label className="grid gap-2 text-sm font-medium text-slate-700">
          Category
          <input
            type="text"
            name="category"
            value={form.category}
            onChange={handleChange}
            required
            className="rounded-xl border border-slate-300 px-4 py-3 font-normal outline-none transition focus:border-sky-500"
            placeholder="e.g. Planning"
          />
        </label>

        <label className="grid gap-2 text-sm font-medium text-slate-700">
          Description
          <textarea
            name="description"
            value={form.description}
            onChange={handleChange}
            rows="4"
            className="rounded-xl border border-slate-300 px-4 py-3 font-normal outline-none transition focus:border-sky-500"
            placeholder="Add a short note for this task"
          />
        </label>

        <button
          type="submit"
          disabled={submitting}
          className="rounded-xl bg-emerald-600 px-5 py-3 font-medium text-white transition hover:bg-emerald-700 disabled:cursor-not-allowed disabled:bg-emerald-300"
        >
          {submitting ? 'Saving...' : 'Save task'}
        </button>
      </form>
    </section>
  );
}

export default TaskForm;
