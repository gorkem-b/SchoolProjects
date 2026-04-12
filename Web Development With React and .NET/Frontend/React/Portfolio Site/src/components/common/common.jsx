import './common.css'

export function Button({ children, type = 'button', variant = 'primary', ...props }) {
	return (
		<button type={type} className={`btn btn-${variant}`} {...props}>
			{children}
		</button>
	)
}

export function Card({ title, children }) {
	return (
		<article className="card">
			{title ? (
				<h3 className="card-title text-xl font-semibold tracking-tight text-(--color-text)">
					{title}
				</h3>
			) : null}
			<div>{children}</div>
		</article>
	)
}

export function Input({ label, id, ...props }) {
	return (
		<label className="field" htmlFor={id}>
			<span>{label}</span>
			<input id={id} {...props} />
		</label>
	)
}

export function SectionTitle({
	eyebrow,
	title,
	description,
	as = 'h2',
	variant = 'section',
	titleClassName = '',
}) {
	const HeadingTag = as
	const isHero = variant === 'hero'
	const baseHeadingClass = isHero
		? 'mt-3 text-5xl font-black leading-tight tracking-tight text-(--color-text) md:text-7xl'
		: 'mt-3 text-3xl font-bold leading-tight tracking-tight text-(--color-text) md:text-4xl'
	const headingClass = `${baseHeadingClass} ${titleClassName}`.trim()
	const descriptionClass = isHero
		? 'mt-4 mx-auto max-w-3xl text-lg leading-relaxed text-(--color-muted)'
		: 'mt-3 max-w-2xl text-base leading-relaxed text-(--color-muted)'

	return (
		<header className={`section-title ${isHero ? 'mb-8 text-center' : 'mb-5'}`}>
			{eyebrow ? (
				<p className="section-eyebrow inline-flex items-center rounded-full border border-(--color-border) bg-(--color-surface) px-3 py-1 text-[0.7rem] font-semibold uppercase tracking-[0.22em] text-(--color-muted)">
					{eyebrow}
				</p>
			) : null}
			<HeadingTag className={headingClass}>
				{title}
			</HeadingTag>
			<div className="mt-3 h-1.5 w-20 rounded-full bg-linear-to-r from-[#0F62FE] via-[#2A9D8F] to-[#E76F51]" />
			{description ? (
				<p className={descriptionClass}>
					{description}
				</p>
			) : null}
		</header>
	)
}
