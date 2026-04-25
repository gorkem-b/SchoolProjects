# Event Reminder Service

A full-stack task and reminder application built with React, ASP.NET Core Web API, Entity Framework Core, MSSQL, and Docker Compose.

The application lets users create, list, update, delete, and filter tasks. A background worker checks due tasks and marks them as notified.

## Table of Contents

- [Features](#features)
- [Tech Stack](#tech-stack)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Data Model](#data-model)
- [API Endpoints](#api-endpoints)
- [Background Worker](#background-worker)
- [Configuration](#configuration)
- [Run with Docker](#run-with-docker)
- [Run Locally Without Docker](#run-locally-without-docker)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)
- [GitHub Readiness](#github-readiness)

## Features

- React dashboard with task summary and upcoming tasks.
- Task CRUD operations:
  - Create task
  - Read/list tasks
  - Update task
  - Delete task
- Filter tasks by:
  - Category
  - Status
  - Notification state
- MSSQL persistence with Entity Framework Core migrations.
- Automatic database migration at backend startup.
- Background reminder worker that checks due pending tasks.
- Docker Compose setup for frontend, backend, and database.
- Health check endpoint for API availability.

## Tech Stack

### Frontend

- React
- Vite
- JavaScript
- Axios
- React Router DOM
- Zustand
- Tailwind CSS
- Nginx for Docker production serving

### Backend

- .NET 10
- ASP.NET Core Web API
- Entity Framework Core
- EF Core SQL Server provider
- Swagger / OpenAPI
- .NET `BackgroundService`

### Database and Infrastructure

- Microsoft SQL Server 2022 Docker image
- Docker
- Docker Compose

## Architecture

The system follows a simple decoupled client-server architecture.

```text
React SPA
   |
   | Axios HTTP requests
   v
ASP.NET Core Web API
   |
   | Entity Framework Core
   v
MSSQL Database

BackgroundService
   |
   | Checks due tasks and updates notification state
   v
MSSQL Database
```

The backend uses a lightweight N-Tier folder structure:

- `Controllers`: HTTP API endpoints.
- `Services`: Business logic and validation.
- `Data`: EF Core `DbContext`.
- `Models`: Entities, DTOs, and status constants.
- `Workers`: Background reminder processing.

## Project Structure

```text
EventReminderService/
├── .dockerignore
├── .env.example
├── .gitignore
├── docker-compose.yml
├── EventReminderService.sln
├── GUIDE.md
├── README.md
├── Backend/
│   └── EventReminder.API/
│       ├── Controllers/
│       │   └── TasksController.cs
│       ├── Data/
│       │   └── AppDbContext.cs
│       ├── Migrations/
│       │   ├── 20260425095116_InitialCreate.cs
│       │   └── AppDbContextModelSnapshot.cs
│       ├── Models/
│       │   ├── TaskDto.cs
│       │   ├── TaskItem.cs
│       │   └── TaskStatuses.cs
│       ├── Properties/
│       │   └── launchSettings.json
│       ├── Services/
│       │   ├── ITaskService.cs
│       │   └── TaskService.cs
│       ├── Workers/
│       │   └── ReminderWorker.cs
│       ├── appsettings.json
│       ├── Dockerfile
│       ├── EventReminder.API.csproj
│       └── Program.cs
└── Frontend/
    ├── src/
    │   ├── api/
    │   │   └── client.js
    │   ├── components/
    │   │   ├── TaskForm.jsx
    │   │   ├── TaskItem.jsx
    │   │   └── TaskList.jsx
    │   ├── pages/
    │   │   ├── Dashboard.jsx
    │   │   └── TasksPage.jsx
    │   ├── store/
    │   │   └── useTaskStore.js
    │   ├── utils/
    │   │   └── formatDate.js
    │   ├── App.jsx
    │   ├── index.css
    │   └── main.jsx
    ├── Dockerfile
    ├── index.html
    ├── nginx.conf
    ├── package.json
    ├── postcss.config.js
    ├── tailwind.config.js
    └── vite.config.js
```

## Data Model

The main database entity is `TaskItem`.

```text
TaskItem
├── Id: int
├── Title: string
├── Description: string?
├── DueDate: DateTime
├── Category: string
├── Status: string
├── IsNotified: bool
├── CreatedAt: DateTime
└── UpdatedAt: DateTime?
```

Supported status values:

```text
Pending
Completed
Notified
```

## API Endpoints

Base API URL:

```text
http://localhost:5000/api
```

### Health Check

```http
GET /health
```

Expected response:

```json
{
  "status": "healthy",
  "service": "EventReminder.API"
}
```

### Tasks

```http
GET    /api/tasks
GET    /api/tasks/{id}
POST   /api/tasks
PUT    /api/tasks/{id}
DELETE /api/tasks/{id}
```

### Filtering

Tasks can be filtered with query parameters:

```http
GET /api/tasks?category=Work&status=Pending&isNotified=false
```

### Create Task Example

```http
POST /api/tasks
Content-Type: application/json
```

```json
{
  "title": "Prepare project demo",
  "description": "Review the event reminder flow",
  "dueDate": "2026-04-25T15:30:00Z",
  "category": "School"
}
```

### Update Task Example

```http
PUT /api/tasks/1
Content-Type: application/json
```

```json
{
  "title": "Prepare project demo",
  "description": "Review and present the final version",
  "dueDate": "2026-04-25T15:30:00Z",
  "category": "School",
  "status": "Completed",
  "isNotified": false
}
```

## Background Worker

The backend registers `ReminderWorker` as a hosted service.

The worker:

1. Runs periodically.
2. Queries tasks where:
   - `DueDate <= DateTime.UtcNow`
   - `Status == Pending`
   - `IsNotified == false`
3. Logs the reminder event.
4. Updates the task:
   - `Status = Notified`
   - `IsNotified = true`

## Configuration

An example environment file is provided:

```text
.env.example
```

Create a local `.env` file if you want to override defaults:

```bash
cp .env.example .env
```

On Windows `cmd.exe`, use:

```cmd
copy .env.example .env
```

Available variables:

```env
MSSQL_SA_PASSWORD=Your_password123!
MSSQL_PID=Developer
MSSQL_DATABASE=EventReminderDb
VITE_API_BASE_URL=http://localhost:5000/api
```

> Do not commit `.env`. It is intentionally ignored by Git.

## Run with Docker

From the project root:

```bash
docker compose up --build
```

Or run in detached mode:

```bash
docker compose up --build -d
```

Useful URLs:

- Frontend: `http://localhost:3000`
- Backend Swagger: `http://localhost:5000/swagger`
- Backend health check: `http://localhost:5000/health`
- MSSQL: `localhost,1433`

To stop containers:

```bash
docker compose down
```

To stop containers and remove the MSSQL volume:

```bash
docker compose down -v
```

Use `down -v` only when you want to reset the local development database.

## Run Locally Without Docker

### Prerequisites

- .NET 10 SDK
- Node.js 20 or newer
- npm
- A running MSSQL instance

### Backend

Update the connection string in:

```text
Backend/EventReminder.API/appsettings.json
```

Then run:

```bash
dotnet restore Backend/EventReminder.API/EventReminder.API.csproj
dotnet run --project Backend/EventReminder.API/EventReminder.API.csproj
```

The backend uses EF Core migrations automatically on startup.

### Frontend

```bash
npm install --prefix Frontend
npm run dev --prefix Frontend
```

Default Vite URL:

```text
http://localhost:5173
```

## Verification

### Backend build

```bash
dotnet build Backend/EventReminder.API/EventReminder.API.csproj
```

### Frontend build

```bash
npm run build --prefix Frontend
```

### Docker Compose validation

```bash
docker compose config
```

### API checks

```bash
curl http://localhost:5000/health
curl http://localhost:5000/api/tasks
```

Expected empty task list response:

```json
[]
```

## Troubleshooting

### Frontend says it cannot reach the API

Check that the backend is running:

```bash
curl http://localhost:5000/health
```

If this fails, check containers:

```bash
docker compose ps
docker logs event-reminder-api
```

### `/api/tasks` returns 500

Check backend logs:

```bash
docker logs event-reminder-api
```

If the log mentions missing database tables, reset the local development database:

```bash
docker compose down -v
docker compose up --build
```

The backend applies EF Core migrations automatically on startup.

### Port already in use

Default ports:

- Frontend: `3000`
- Backend: `5000`
- MSSQL: `1433`

Stop the conflicting process or change the port mappings in `docker-compose.yml`.

## GitHub Readiness

The repository is prepared so generated files and local secrets are not committed.

Ignored files include:

- `bin/`
- `obj/`
- `node_modules/`
- `dist/`
- `.env`
- logs and local IDE files

Before pushing, verify:

```bash
git status --short
dotnet build Backend/EventReminder.API/EventReminder.API.csproj
npm run build --prefix Frontend
docker compose config
```

Then commit and push:

```bash
git add .
git commit -m "Add event reminder full-stack application"
git push
```
