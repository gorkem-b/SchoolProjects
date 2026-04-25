# Book Library Frontend

React single-page application for the Book Library project. It provides screens for managing books and reading tasks and communicates with the .NET API through Axios.

## Stack

- React
- React Router
- Axios
- Vite

## Scripts

```bash
npm install
npm run dev
npm run build
npm run lint
```

## API Configuration

During local development, Vite proxies `/api` requests to `http://localhost:5046`.

For a custom API URL, set `VITE_API_BASE_URL` before building or running the app:

```bash
VITE_API_BASE_URL=http://localhost:5046/api npm run dev
```

In Docker, Nginx serves the static frontend and proxies `/api` requests to the backend service.
