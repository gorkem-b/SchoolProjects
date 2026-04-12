import {
	ContactSection,
	ExperienceSection,
	HeroSection,
	ProjectsSection,
	SkillsSection,
} from '../components/sections/sections'
import { useScroll } from '../hooks/useScroll'
import { useTheme } from '../hooks/useTheme'
import { Button } from '../components/common/common'

function Home() {
	const scrollY = useScroll()
	const { theme, toggleTheme } = useTheme()

	return (
		<div className="stack">
			<div className="row-between">
				<small>Scroll: {Math.round(scrollY)}px</small>
				<Button variant="ghost" onClick={toggleTheme}>
					Theme: {theme}
				</Button>
			</div>

			<HeroSection />
			<ProjectsSection />
			<SkillsSection />
			<ExperienceSection />
			<ContactSection />
		</div>
	)
}

export default Home
