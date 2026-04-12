import { describe, it, expect } from 'vitest';
import { render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import App from './App';

describe('App', () => {
  it('sayaçlar ve üst kartlarla başlangıç oyun durumunu render eder', () => {
    const { container } = render(<App />);

    expect(container.querySelector('.move-count')).toHaveTextContent('0');
    expect(container.querySelector('.score-count')).toHaveTextContent('0');
    expect(container.querySelectorAll('.card.hidden')).toHaveLength(13);
  });

  it('iki farklı gruptan kart seçildiğinde hamle sayısını artırır', async () => {
    const user = userEvent.setup();
    const { container } = render(<App />);

    const hiddenCards = container.querySelectorAll('.card.hidden');
    await user.click(hiddenCards[0]);
    await user.click(hiddenCards[1]);

    await waitFor(() => {
      expect(container.querySelector('.move-count')).toHaveTextContent('1');
    });
  });

  it('yeniden başlat butonuna tıklandığında skor ve hamleyi sıfırlar', async () => {
    const user = userEvent.setup();
    const { container } = render(<App />);

    const hiddenCards = container.querySelectorAll('.card.hidden');
    await user.click(hiddenCards[0]);
    await user.click(hiddenCards[1]);

    await waitFor(() => {
      expect(container.querySelector('.move-count')).toHaveTextContent('1');
    });

    await user.click(screen.getByRole('button', { name: /restart game/i }));

    expect(container.querySelector('.move-count')).toHaveTextContent('0');
    expect(container.querySelector('.score-count')).toHaveTextContent('0');
  });
});
