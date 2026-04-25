function EmptyState({ title, text }) {
  return (
    <div className="empty-state">
      <h3 className="empty-state__title">{title}</h3>
      <p className="empty-state__text">{text}</p>
    </div>
  )
}

export default EmptyState