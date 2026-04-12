import { useMemo, useState } from 'react'
import { Link } from 'react-router-dom'
import experience from '../../data/experience.json'
import projects from '../../data/projects.json'
import skills from '../../data/skills.json'
import capabilityAIcon from '../../assets/icons/capability-a.svg'
import capabilityBIcon from '../../assets/icons/capability-b.svg'
import capabilityCIcon from '../../assets/icons/capability-c.svg'
import capabilityDIcon from '../../assets/icons/capability-d.svg'
import projectAlphaImage from '../../assets/images/project-alpha.svg'
import projectOrbitImage from '../../assets/images/project-orbit.svg'
import projectNovaImage from '../../assets/images/project-nova.svg'
import projectLatticeImage from '../../assets/images/project-lattice.svg'
import { formatDate } from '../../utils/formatDate'
import { isRequired, isValidEmail } from '../../utils/validation'
import { Button, Card, Input, SectionTitle } from '../common/common'
import './sections.css'

const skillIconMap = {
	'capability-a': capabilityAIcon,
	'capability-b': capabilityBIcon,
	'capability-c': capabilityCIcon,
	'capability-d': capabilityDIcon,
}

const projectImageMap = {
	'project-alpha': projectAlphaImage,
	'project-orbit': projectOrbitImage,
	'project-nova': projectNovaImage,
	'project-lattice': projectLatticeImage,
}

export function HeroSection() {
	return (
		<section className="hero">
			<SectionTitle
				eyebrow="Portfolio"
				title="Frontend Developer"
				description="Building scalable interfaces with React and modern CSS architecture."
				as="h1"
				variant="hero"
				titleClassName="font-bold"
			/>
		</section>
	)
}

export function ProjectsSection() {
	return (
		<section>
			<SectionTitle eyebrow="Work" title="Projects" />
			<div className="grid two-col">
				{projects.map((project) => (
					<Card key={project.slug} title={project.title}>
						<img
							src={projectImageMap[project.slug]}
							className="project-thumb"
							alt={`${project.title} preview`}
						/>
						<p>{project.description}</p>
						<ul className="badge-list" aria-label={`${project.title} technologies`}>
							{project.tech.map((tech) => (
								<li key={tech}>{tech}</li>
							))}
						</ul>
						<p>
							<Link to={`/projects/${project.slug}`}>View details</Link>
						</p>
					</Card>
				))}
			</div>
		</section>
	)
}

export function SkillsSection() {
	return (
		<section>
			<SectionTitle eyebrow="Stack" title="Skills" />
			<ul className="badge-list skill-list" aria-label="Skills list">
				{skills.map((item) => (
					<li key={item.name} className="skill-chip">
						<img
							src={skillIconMap[item.icon]}
							className="skill-icon"
							alt=""
							aria-hidden="true"
						/>
						{item.name} - {item.level}
					</li>
				))}
			</ul>
		</section>
	)
}

export function ExperienceSection() {
	return (
		<section>
			<SectionTitle eyebrow="Career" title="Experience" />
			<div className="stack">
				{experience.map((item) => (
					<Card key={`${item.company}-${item.role}`} title={`${item.role} · ${item.company}`}>
						<p>
							{formatDate(item.startDate)} - {formatDate(item.endDate)}
						</p>
						<p>{item.summary}</p>
					</Card>
				))}
			</div>
		</section>
	)
}

export function ContactSection() {
	const [name, setName] = useState('')
	const [email, setEmail] = useState('')
	const [isSubmitted, setIsSubmitted] = useState(false)

	const errors = useMemo(
		() => ({
			name: isSubmitted && !isRequired(name),
			email: isSubmitted && !isValidEmail(email),
		}),
		[email, isSubmitted, name],
	)

	function onSubmit(event) {
		event.preventDefault()
		setIsSubmitted(true)

		if (!errors.name && !errors.email) {
			setName('')
			setEmail('')
			setIsSubmitted(false)
		}
	}

	return (
		<section>
			<SectionTitle eyebrow="Contact" title="Let's talk" />
			<form className="contact-form" onSubmit={onSubmit} noValidate>
				<Input
					id="name"
					label="Name"
					value={name}
					onChange={(event) => setName(event.target.value)}
					placeholder="Your name"
				/>
				{errors.name ? <p className="error">Name is required.</p> : null}

				<Input
					id="email"
					label="Email"
					type="email"
					value={email}
					onChange={(event) => setEmail(event.target.value)}
					placeholder="name@example.com"
				/>
				{errors.email ? <p className="error">Enter a valid email address.</p> : null}

				<Button type="submit">Send message</Button>
			</form>
		</section>
	)
}
