# Task Manager API & UI

## Overview

This project is a full-stack task management application built with a React frontend and an ASP.NET Core Web API backend. It supports user registration and login with JWT-based authentication, then allows each authenticated user to create, view, update, delete, and filter only their own tasks.

The current implementation is designed to run locally with Docker Compose. The stack includes a React UI, a .NET 10 API, and a SQL Server container. Database migrations are automatically applied when the API starts inside Docker.

---

## Current Features

### Authentication
- User registration
- User login
- JWT token generation
- Protected API endpoints
- Token persistence in the frontend

### Task Management
- Create tasks
- List tasks
- Update tasks
- Delete tasks
- Filter tasks by status
- Filter tasks by category
- User-specific task isolation

### UX Improvements
- Inline error feedback on auth screens
- Form inputs stay intact after failed requests
- Edit mode with cancel action
- Success messages with auto-dismiss behavior
- Delete confirmation before removing a task
- Empty-state and filtered-empty-state handling

### Infrastructure
- SQL Server running in Docker
- ASP.NET Core API running in Docker
- React UI served through Nginx in Docker
- EF Core migrations applied automatically on startup

---

## Tech Stack

- **Frontend:** React, React Router DOM, Axios, Vite
- **Backend:** ASP.NET Core Web API (.NET 10)
- **Database:** Microsoft SQL Server
- **ORM:** Entity Framework Core
- **Authentication:** JWT (JSON Web Token)
- **Containerization:** Docker, Docker Compose

---

## Architecture

The application follows a client-server architecture:

- The **React frontend** handles routing, authentication state, forms, and task management screens.
- The **.NET API** handles authentication, authorization, task business logic, and database access.
- **SQL Server** stores users and tasks.

The backend is organized into clear layers:

1. **Controllers**
   - Receive HTTP requests
   - Validate request payloads
   - Return HTTP responses

2. **Services**
   - Handle business rules
   - Create JWT tokens
   - Hash and verify passwords
   - Enforce user-specific task access

3. **Data Layer**
   - Uses Entity Framework Core through `AppDbContext`
   - Maps entities to the database
   - Applies migrations

---

## Application Flow

### Authentication Flow

1. A user registers or logs in from the React UI.
2. The frontend sends credentials to the API.
3. The API validates the user.
4. On successful login, the API returns a JWT.
5. The frontend stores the token and includes it in future requests.

### Task Flow

1. The frontend sends authenticated requests with `Authorization: Bearer <token>`.
2. The API validates the token and extracts the user identity.
3. The API only returns or modifies tasks that belong to that user.
4. The frontend updates local state and reflects changes in the UI.

---

## Project Structure

### Backend

```text
TaskApi/
├── Controllers/
│   ├── AuthController.cs
│   └── TaskController.cs
├── Data/
│   └── AppDbContext.cs
├── DTOs/
│   ├── UserAuthDto.cs
│   ├── TaskCreateUpdateDto.cs
│   └── TaskResponseDto.cs
├── Migrations/
├── Models/
│   ├── User.cs
│   └── TaskItem.cs
├── Services/
│   ├── AuthService.cs
│   └── TaskService.cs
├── appsettings.json
├── Dockerfile
└── Program.cs
```

### Frontend

```text
task-ui/
├── src/
│   ├── api/
│   │   ├── authService.js
│   │   ├── axiosConfig.js
│   │   └── taskService.js
│   ├── components/
│   │   ├── Navbar.jsx
│   │   ├── ProtectedRoute.jsx
│   │   ├── TaskForm.jsx
│   │   └── TaskItem.jsx
│   ├── context/
│   │   └── AuthContext.jsx
│   ├── pages/
│   │   ├── Dashboard.jsx
│   │   ├── Login.jsx
│   │   └── Register.jsx
│   ├── App.jsx
│   ├── index.css
│   └── main.jsx
├── .env
├── Dockerfile
├── index.html
├── nginx.conf
└── package.json
```

---

## Authentication Details

- Passwords are hashed in the backend before they are stored.
- JWT tokens include the authenticated user's identity.
- Protected task endpoints require a valid token.
- The frontend stores the token in `localStorage`.
- Axios automatically attaches the token to outgoing API requests.

---

## Task Management Details

Each authenticated user can:

- create a task
- view their own tasks
- update their own tasks
- delete their own tasks
- filter tasks by status and category

Tasks include:

- `title`
- `description`
- `category`
- `status`
- `userId`

---

## Docker-Based Local Setup

The project is configured to run with Docker Compose.

### Services

- **sqlserver** → SQL Server database
- **taskapi** → ASP.NET Core Web API
- **taskui** → React frontend served through Nginx

### Important Ports

- **UI:** `http://localhost:4173`
- **API:** `http://localhost:8080/api`
- **SQL Server:** `localhost:1433`

### Start the Project

1. Create a local environment file from the example:

```bash
copy .env.example .env
```

2. Update the values inside `.env` with your own local secrets.

3. Start the stack:

```bash
docker compose up --build -d
```

### Stop the Project

```bash
docker compose down
```

---

## Running Without Docker

### Backend

```bash
dotnet run --project .\TaskApi\TaskApi.csproj
```

### Frontend

```bash
npm install --prefix .\task-ui
npm run dev --prefix .\task-ui
```

> Note: the current default configuration is optimized for Docker-based local development.
> Do not commit real secrets to source control. Use `.env` for local values.

---

## Build Commands

### Backend

```bash
dotnet build .\TaskApi\TaskApi.csproj
```

### Frontend

```bash
npm run build --prefix .\task-ui
```

---

## API Summary

### Auth Endpoints

- `POST /api/auth/register`
- `POST /api/auth/login`

### Task Endpoints

- `GET /api/tasks`
- `POST /api/tasks`
- `PUT /api/tasks/{id}`
- `DELETE /api/tasks/{id}`

All task endpoints require a valid JWT token.

---

## Current Status

The main scope of the project is implemented and working:

- authentication works
- task CRUD works
- filtering works
- frontend and backend builds succeed
- Docker Compose setup works
- SQL Server migrations run automatically in Docker
- the API flow has been tested end-to-end

Possible future improvements:

- further UI polish
- stronger validation rules
- automated tests
- README examples with sample request payloads