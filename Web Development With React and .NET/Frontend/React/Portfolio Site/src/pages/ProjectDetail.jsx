import { Link, useParams } from 'react-router-dom'
import projects from '../data/projects.json'
import projectAlphaImage from '../assets/images/project-alpha.svg'
import projectOrbitImage from '../assets/images/project-orbit.svg'
import projectNovaImage from '../assets/images/project-nova.svg'
import projectLatticeImage from '../assets/images/project-lattice.svg'
import { Card, SectionTitle } from '../components/common/common'
import '../components/sections/sections.css'

const projectImageMap = {
	'project-alpha': projectAlphaImage,
	'project-orbit': projectOrbitImage,
	'project-nova': projectNovaImage,
	'project-lattice': projectLatticeImage,
}

function ProjectDetail() {
	const { slug } = useParams()
	const project = projects.find((item) => item.slug === slug)

	if (!project) {
		return (
			<section>
				<SectionTitle title="Project not found" />
				<p>The requested project does not exist.</p>
				<p>
					<Link to="/">Back to home</Link>
				</p>
			</section>
		)
	}

	return (
		<section>
			<SectionTitle
				eyebrow="Project"
				title={project.title}
				description={project.description}
				as="h1"
				variant="hero"
			/>
			<img
				src={projectImageMap[project.slug]}
				alt={`${project.title} preview`}
				className="project-thumb"
			/>
			<Card title="Tech Stack">
				<ul className="badge-list">
					{project.tech.map((tech) => (
						<li key={tech}>{tech}</li>
					))}
				</ul>
			</Card>
			<p>
				<a href={project.demo} target="_blank" rel="noreferrer">
					Live Demo
				</a>
			</p>
			<p>
				<a href={project.repo} target="_blank" rel="noreferrer">
					Repository
				</a>
			</p>
		</section>
	)
}

export default ProjectDetail
