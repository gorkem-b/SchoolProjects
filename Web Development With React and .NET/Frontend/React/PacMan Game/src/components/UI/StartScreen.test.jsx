import { render } from '@testing-library/react'
import { StartScreen } from './StartScreen'

describe('StartScreen', () => {
  test('renders consistently', () => {
    const { container } = render(<StartScreen onStart={() => {}} />)

    expect(container.firstChild).toMatchInlineSnapshot(`
<div
  class="overlay"
>
  <div
    class="panel"
  >
    <img
      alt="Pacman Arena logo"
      class="panelLogo"
      src="/logo.svg"
    />
    <h1>
      Pacman Arena
    </h1>
    <p>
      Use the arrow keys to move. Press Enter to start.
    </p>
    <button
      class="actionButton"
      type="button"
    >
      Start Game
    </button>
  </div>
</div>
`)
  })
})
