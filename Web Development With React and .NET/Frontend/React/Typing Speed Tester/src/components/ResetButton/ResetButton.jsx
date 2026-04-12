import styles from './ResetButton.module.css'

function ResetButton({ onReset }) {
  return (
    <button type="button" onClick={onReset} className={styles.button}>
      New Test
    </button>
  )
}

export default ResetButton
