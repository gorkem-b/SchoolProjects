# Portfolio Site

## Project Structure

```text
src/
├── components/     # Reusable React components
├── pages/          # Page components
├── styles/         # CSS/styling
├── App.jsx         # Main app component
└── main.jsx        # Entry point
```

## Folder and File Responsibilities

### public/

Static files that will not be compiled. They are copied directly to the root directory during the build.

- `favicon.svg`: Site tab icon.
- `robots.txt`: Search engine directives.
- `manifest.json`: PWA configuration.

### src/assets/

Media and static assets.

- `images/`: Photos used in the project (`.png`, `.webp`).
- `fonts/`: Custom web fonts used in the project (`.woff2`).
- `icons/`: Icons used within the site.

### src/components/

Reusable UI components.

- `common/`: General components such as Button, Input, Card, Modal.
- `layout/`: Header, Footer, Sidebar, Grid wrapper.
- `sections/`: Page sections like Hero, About, Projects, Contact, Skills.
- File types: `.jsx` (React component), `.css` (component‑specific styles).

### src/pages/

Pages to be routed.

- `Home.jsx`: Main landing page.
- `ProjectDetail.jsx`: Detailed view of a project.
- `NotFound.jsx`: 404 page.

### src/hooks/

Custom React hooks.

- `useScroll.js`: Scroll position tracking.
- `useTheme.js`: Dark/light mode management.
- `useFetch.js`: API data fetching logic.

### src/utils/

Helper pure JavaScript functions.

- `formatDate.js`: Date formatting.
- `validation.js`: Form validation logic.
- `constants.js`: Constant values (`API_URL`, `ROUTES`).

### src/data/

Static content data.

- `projects.json`: Project list, descriptions, links.
- `skills.json`: Skills list, icon mappings.
- `experience.json`: Work history data.

### src/styles/

Global style definitions.

- `global.css`: Reset CSS, `body` element styles.
- `variables.css`: CSS variables (`--primary-color`, `--font-main`).
- `mixins.css`: Reusable style patterns.

### src/App.jsx

Route management (React Router), main layout wrapping.

### src/main.jsx

Application entry point, DOM rendering, provider wrapping.

### package.json

Dependency management, scripts (`dev`, `build`, `preview`).

### vite.config.js

Build configuration, alias definitions, proxy settings.

## Projects & Work Experience

Create anon projects, anon work experience.

## License

This project is open source and available under the MIT License.
