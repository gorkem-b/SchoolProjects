# Book Library API

A full-stack digital library application for managing books and reading tasks. The frontend is a React single-page application, the backend is a layered .NET Web API, and persistence is handled with SQL Server through Entity Framework Core.

## Features

- Add, list, and delete books.
- Create, update, filter, and delete reading tasks.
- Associate each reading task with a book.
- Track task status as `ToRead` or `Completed`.
- Run the complete stack with Docker Compose.
- Explore the API with Swagger UI.

## Tech Stack

| Area | Technology |
| --- | --- |
| Frontend | React, React Router, Axios, Vite |
| Backend | .NET 10 Web API, Entity Framework Core |
| Database | Microsoft SQL Server |
| DevOps | Docker, Docker Compose, Nginx |

## Architecture

The backend follows a simple layered architecture:

1. **API layer** (`BookLibrary.API`) handles HTTP requests and responses.
2. **Business layer** (`BookLibrary.Business`) contains service interfaces, service implementations, and DTO mapping logic.
3. **Data access layer** (`BookLibrary.DataAccess`) contains EF Core `DbContext`, repositories, and migrations.
4. **Core layer** (`BookLibrary.Core`) contains entities, DTOs, and enums.

The React frontend communicates with the API through Axios. In Docker, Nginx serves the built frontend and proxies `/api` requests to the backend container.

## Folder Structure

```text
BookLibraryAPI/
├── docker-compose.yml
├── backend/
│   ├── BookLibrary.slnx
│   ├── BookLibrary.API/
│   │   ├── Controllers/
│   │   ├── Dockerfile
│   │   ├── Program.cs
│   │   └── appsettings.json
│   ├── BookLibrary.Business/
│   │   ├── Interfaces/
│   │   ├── Mappings/
│   │   └── Services/
│   ├── BookLibrary.Core/
│   │   ├── DTOs/
│   │   ├── Entities/
│   │   └── Enums/
│   └── BookLibrary.DataAccess/
│       ├── Context/
│       ├── Migrations/
│       └── Repositories/
└── frontend/
    ├── Dockerfile
    ├── nginx.conf
    ├── package.json
    └── src/
        ├── api/
        ├── components/
        ├── constants/
        └── pages/
```

## API Endpoints

### Books

| Method | Endpoint | Description |
| --- | --- | --- |
| `GET` | `/api/books` | List all books with their tasks. |
| `GET` | `/api/books/{id}` | Get a single book by id. |
| `POST` | `/api/books` | Create a new book. |
| `PUT` | `/api/books/{id}` | Update an existing book. |
| `DELETE` | `/api/books/{id}` | Delete a book and its tasks. |

### Tasks

| Method | Endpoint | Description |
| --- | --- | --- |
| `GET` | `/api/tasks` | List all reading tasks. |
| `GET` | `/api/tasks/{id}` | Get a single task by id. |
| `POST` | `/api/tasks` | Create a new reading task. |
| `PUT` | `/api/tasks/{id}` | Update a task title or status. |
| `DELETE` | `/api/tasks/{id}` | Delete a reading task. |

## Run with Docker Compose

Prerequisites:

- Docker Desktop

From the project root:

```bash
docker compose up --build
```

Services:

- Frontend: <http://localhost:3000>
- Backend API: <http://localhost:5046>
- Swagger UI: <http://localhost:5046/swagger>
- SQL Server: `localhost,1433`

The backend applies EF Core migrations automatically on startup.

## Local Development

Prerequisites:

- .NET 10 SDK
- Node.js
- SQL Server or the SQL Server Docker service from `docker-compose.yml`

Start SQL Server only:

```bash
docker compose up db
```

Run the backend:

```bash
dotnet run --project backend/BookLibrary.API/BookLibrary.API.csproj
```

Run the frontend:

```bash
cd frontend
npm install
npm run dev
```

The Vite development server proxies `/api` requests to `http://localhost:5046`.

## Configuration

The default local connection string is stored in `backend/BookLibrary.API/appsettings.json`:

```text
Server=localhost,1433;Database=BookLibraryDb;User Id=sa;Password=Your_password123;TrustServerCertificate=True;Encrypt=False
```

Docker Compose overrides this value with:

```text
Server=db,1433;Database=BookLibraryDb;User Id=sa;Password=Your_password123;TrustServerCertificate=True;Encrypt=False
```

For real deployments, replace the sample password with a secure secret management approach.

## Verification Commands

```bash
dotnet build backend/BookLibrary.slnx

cd frontend
npm run build

cd ..
docker compose config
```

## Notes for GitHub

- Build outputs such as `bin/`, `obj/`, `dist/`, and `node_modules/` are ignored.
- Docker build contexts use `.dockerignore` files for smaller images.
- The repository includes EF Core migrations so the database schema can be created automatically.