export default function TaskItem({ task, onEdit, onDelete, isDeleting = false, isEditing = false }) {
  const statusClassName = `status-pill status-pill-${task.status?.toLowerCase() ?? "pending"}`;

  return (
    <article className="card task-item">
      <div>
        <h3>{task.title}</h3>
        <p className="muted">{task.description || "No description"}</p>
      </div>

      <div className="task-meta-row">
        <span className={statusClassName}>{task.status}</span>
        <span className="task-category">{task.category || "Uncategorized"}</span>
      </div>

      <div className="task-actions">
        <button className="button button-secondary" type="button" onClick={() => onEdit?.(task)} disabled={isDeleting}>
          {isEditing ? "Editing..." : "Edit"}
        </button>
        <button className="button" type="button" onClick={() => onDelete?.(task.id)} disabled={isDeleting}>
          {isDeleting ? "Deleting..." : "Delete"}
        </button>
      </div>
    </article>
  );
}