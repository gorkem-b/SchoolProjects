import styles from './Stats.module.css'

function Stats({ stats }) {
  const items = [
    { label: 'WPM', value: stats.wpm },
    { label: 'CPM', value: stats.cpm },
    { label: 'Errors', value: stats.errors },
    { label: 'Accuracy', value: `${stats.accuracy}%` },
    { label: 'Words', value: stats.words },
    { label: 'Characters', value: stats.totalChars },
  ]

  return (
    <section className={styles.grid} aria-label="Statistics">
      {items.map((item) => (
        <article key={item.label} className={styles.card}>
          <span>{item.label}</span>
          <strong>{item.value}</strong>
        </article>
      ))}
    </section>
  )
}

export default Stats
