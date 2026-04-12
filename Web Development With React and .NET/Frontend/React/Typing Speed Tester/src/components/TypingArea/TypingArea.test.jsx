import { render, screen } from '@testing-library/react'
import { describe, expect, it } from 'vitest'
import TypingArea from './TypingArea'

describe('TypingArea', () => {
  it('renders a textarea for typing', () => {
    render(
      <TypingArea
        text="hello"
        userInput=""
        isFinished={false}
        onInputChange={() => {}}
      />,
    )

    expect(screen.getByLabelText('Start typing')).toBeTruthy()
  })
})
