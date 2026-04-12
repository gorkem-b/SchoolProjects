import { consumePellet } from './useCollision'

describe('consumePellet', () => {
  test('consumes standard pellet and returns 10 points', () => {
    const map = [
      [1, 1, 1],
      [1, 2, 1],
      [1, 1, 1],
    ]

    const score = consumePellet(map, { x: 1, y: 1 })

    expect(score).toBe(10)
    expect(map[1][1]).toBe(0)
  })

  test('consumes power pellet and returns 50 points', () => {
    const map = [
      [1, 1, 1],
      [1, 3, 1],
      [1, 1, 1],
    ]

    const score = consumePellet(map, { x: 1, y: 1 })

    expect(score).toBe(50)
    expect(map[1][1]).toBe(0)
  })
})
