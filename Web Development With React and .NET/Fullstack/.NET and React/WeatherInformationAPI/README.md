# Weather Information API

Weather Information API is a full-stack weather-driven task planning app built with **React**, **Zustand**, **.NET 10**, **SQL Server**, and **Docker**.

The app lets a user search the current weather for a city, cache the result on the backend for 10 minutes, and turn that weather context into a task that can be stored, listed, updated, and deleted.

## Features

- Search current weather by city
- Use **Open-Meteo** for geocoding and forecast data
- No external API key required
- Cache weather responses for 10 minutes with `IMemoryCache`
- Create, list, update, and delete weather-related tasks
- Persist tasks in **Microsoft SQL Server**
- Run the full stack with **Docker Compose**
- Use **Nginx** as the frontend web server and `/api` reverse proxy

## Tech Stack

### Frontend
- React 18
- Vite
- Tailwind CSS
- Axios
- Zustand
- Nginx

### Backend
- .NET 10 Web API
- C#
- Entity Framework Core 10
- IMemoryCache
- HttpClient

### Database
- Microsoft SQL Server 2022

### Infrastructure
- Docker
- Docker Compose

## Architecture

The project follows a simple 3-layer structure:

1. **Frontend (React)**
   - Collects user input
   - Displays weather data and tasks
   - Talks to the backend through `/api`

2. **Backend (.NET Web API)**
   - Handles routing and business logic
   - Fetches weather data from Open-Meteo
   - Caches weather responses
   - Stores tasks in SQL Server

3. **Database (SQL Server)**
   - Persists task records only

## Project Structure

```text
WeatherInformationAPI/
├── docker-compose.yml
├── .env.example
├── backend/
│   ├── Controllers/
│   ├── Data/
│   ├── DTOs/
│   ├── Models/
│   ├── Services/
│   ├── Dockerfile
│   ├── Program.cs
│   └── WeatherInformationAPI.csproj
└── frontend/
    ├── src/
    │   ├── components/
    │   ├── services/
    │   ├── store/
    │   ├── App.jsx
    │   └── main.jsx
    ├── Dockerfile
    ├── nginx.conf
    └── package.json
```

## Getting Started

### Option 1: Run with Docker

This is the recommended way.

1. Create a local environment file if you want to override the default SQL Server password:

   ```bash
   copy .env.example .env
   ```

2. Start the full stack:

   ```bash
   docker compose up --build
   ```

3. Open the app:

   - Frontend: `http://localhost:3000`
   - Backend Swagger: `http://localhost:8080/swagger`

### Option 2: Run locally for development

#### Requirements
- .NET 10 SDK
- Node.js 20+
- npm
- Docker Desktop recommended for SQL Server

#### Steps

1. Start SQL Server only:

   ```bash
   docker compose up -d sqlserver
   ```

2. Run the backend:

   ```bash
   dotnet run --project backend/WeatherInformationAPI.csproj
   ```

3. Run the frontend:

   ```bash
   cd frontend
   npm install
   npm run dev
   ```

4. Open the Vite dev server:

   - Frontend: `http://localhost:5173`

In development mode, Vite proxies `/api` requests to `http://localhost:8080`.

## Environment Variables

The project does not require any external weather API key.

### Supported variables

```env
MSSQL_SA_PASSWORD=YourStrong!Passw0rd
```

## API Endpoints

### Weather

- `GET /api/weather/{city}`

Example:

```http
GET /api/weather/Istanbul
```

Example response:

```json
{
  "cityName": "Istanbul",
  "country": "Turkey",
  "description": "Overcast",
  "icon": "☁️",
  "temperature": 17.3,
  "feelsLike": 14.1,
  "humidity": 48,
  "windSpeed": 15.5
}
```

### Tasks

- `GET /api/tasks`
- `GET /api/tasks/{id}`
- `POST /api/tasks`
- `PUT /api/tasks/{id}`
- `DELETE /api/tasks/{id}`

Example create payload:

```json
{
  "title": "Prepare for rainy weather",
  "description": "Take an umbrella before leaving home.",
  "category": "Planning",
  "cityName": "Istanbul"
}
```

Task status is managed with the `WeatherTaskStatus` enum:

- `Pending`
- `Completed`

## Verification

The project has been verified with:

- `dotnet build backend/WeatherInformationAPI.csproj`
- `npm run build` inside `frontend`
- `docker compose up --build -d`
- `GET /api/tasks` → `200 OK`
- `GET /api/weather/Istanbul` → `200 OK`
- `GET http://localhost:3000/api/weather/Istanbul` → `200 OK`

## Notes for This Repository

This project lives inside a larger repository. If you want to commit only this app, stage only the `WeatherInformationAPI` folder and avoid unrelated changes from other project directories.