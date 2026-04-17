import { useEffect, useState } from "react";

const emptyFormState = {
  title: "",
  category: "",
  description: "",
  status: "Pending",
};

function toFormState(task) {
  if (!task) {
    return emptyFormState;
  }

  return {
    title: task.title ?? "",
    category: task.category ?? "",
    description: task.description ?? "",
    status: task.status ?? "Pending",
  };
}

export default function TaskForm({
  onSubmit,
  submitLabel = "Save",
  isSubmitting = false,
  initialValues = null,
  title = "Task Form",
  onCancel,
  showCancel = false,
  onInteraction,
}) {
  const [form, setForm] = useState(() => toFormState(initialValues));

  useEffect(() => {
    setForm(toFormState(initialValues));
  }, [initialValues]);

  const handleChange = (event) => {
    const { name, value } = event.target;
    setForm((current) => ({ ...current, [name]: value }));
    onInteraction?.();
  };

  const handleSubmit = async (event) => {
    event.preventDefault();

    const normalizedForm = {
      ...form,
      title: form.title.trim(),
      category: form.category.trim(),
      description: form.description.trim(),
      status: form.status,
    };

    const result = await onSubmit?.(normalizedForm);

    if (!initialValues && result !== false) {
      setForm(emptyFormState);
    }
  };

  return (
    <form className="card form-stack" onSubmit={handleSubmit}>
      <h2>{title}</h2>
      <p className="muted form-helper">
        {initialValues ? "Make your changes and save them below." : "Fill the form to add a new task."}
      </p>
      <input
        name="title"
        placeholder="Title"
        value={form.title}
        onChange={handleChange}
        disabled={isSubmitting}
      />
      <input
        name="category"
        placeholder="Category"
        value={form.category}
        onChange={handleChange}
        disabled={isSubmitting}
      />
      <textarea
        name="description"
        placeholder="Description"
        rows="4"
        value={form.description}
        onChange={handleChange}
        disabled={isSubmitting}
      />
      <select name="status" value={form.status} onChange={handleChange} disabled={isSubmitting}>
        <option value="Pending">Pending</option>
        <option value="InProgress">In Progress</option>
        <option value="Completed">Completed</option>
      </select>
      <button className="button" type="submit" disabled={isSubmitting || !form.title.trim()}>
        {submitLabel}
      </button>

      {showCancel && (
        <button className="button button-secondary" type="button" onClick={onCancel} disabled={isSubmitting}>
          Cancel edit
        </button>
      )}
    </form>
  );
}