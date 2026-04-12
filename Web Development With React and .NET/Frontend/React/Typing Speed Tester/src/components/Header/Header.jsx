import styles from './Header.module.css'

function Header() {
  return (
    <header className={styles.header}>
      <h1>Typing Speed Tester</h1>
      <p>
        The 60-second test starts when you begin typing in the input area. Your
        goal is to type with the highest possible accuracy.
      </p>
    </header>
  )
}

export default Header
