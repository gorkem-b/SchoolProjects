import { Route, Routes } from 'react-router-dom'
import { PageLayout } from './components/layout/layout'
import Home from './pages/Home'
import NotFound from './pages/NotFound'
import ProjectDetail from './pages/ProjectDetail'
import { ROUTES } from './utils/constants'

function App() {
  return (
    <Routes>
      <Route
        path={ROUTES.home}
        element={
          <PageLayout>
            <Home />
          </PageLayout>
        }
      />

      <Route
        path={ROUTES.projectDetail}
        element={
          <PageLayout>
            <ProjectDetail />
          </PageLayout>
        }
      />

      <Route
        path={ROUTES.notFound}
        element={
          <PageLayout>
            <NotFound />
          </PageLayout>
        }
      />
    </Routes>
  )
}

export default App
