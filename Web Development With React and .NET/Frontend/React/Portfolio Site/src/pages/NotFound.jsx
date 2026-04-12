import { Link } from 'react-router-dom'

function NotFound() {
	return (
		<section className="space-y-4 py-10 text-center">
			<h1 className="text-6xl font-black tracking-tight md:text-8xl">
				<span className="bg-linear-to-r from-[#0F62FE] via-[#2A9D8F] to-[#E76F51] bg-clip-text text-transparent">
					404
				</span>
			</h1>
			<p className="mx-auto max-w-xl text-lg text-(--color-muted)">
				The page you requested could not be found.
			</p>
			<p>
				<Link
					to="/"
					className="inline-flex items-center rounded-full border border-(--color-border) bg-(--color-surface) px-4 py-2 text-sm font-semibold text-(--color-text) transition hover:-translate-y-0.5"
				>
					Back to home
				</Link>
			</p>
		</section>
	)
}

export default NotFound
