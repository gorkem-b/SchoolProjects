import { Link, NavLink } from 'react-router-dom'
import { ROUTES } from '../../utils/constants'
import socialCodeIcon from '../../assets/icons/social-code.svg'
import socialNetworkIcon from '../../assets/icons/social-network.svg'
import contactMailIcon from '../../assets/icons/contact-mail.svg'
import contactChatIcon from '../../assets/icons/contact-chat.svg'
import './layout.css'

function Header() {
	return (
		<header className="site-header">
			<nav>
				<Link className="brand" to={ROUTES.home}>
					Anonymous Portfolio
				</Link>
				<NavLink to={ROUTES.home}>Home</NavLink>
				<div className="social-links" aria-label="External links">
					<a href="https://github.com/example" target="_blank" rel="noreferrer" aria-label="Code profile">
						<img src={socialCodeIcon} alt="" aria-hidden="true" />
					</a>
					<a href="https://www.linkedin.com/in/example" target="_blank" rel="noreferrer" aria-label="Network profile">
						<img src={socialNetworkIcon} alt="" aria-hidden="true" />
					</a>
				</div>
			</nav>
		</header>
	)
}

function Footer() {
	return (
		<footer className="site-footer">
			<small>© {new Date().getFullYear()} Portfolio Owner. All rights reserved.</small>
			<div className="footer-links" aria-label="Contact links">
				<a href="mailto:hello@example.com" aria-label="Send email">
					<img src={contactMailIcon} alt="" aria-hidden="true" />
					<span>hello@example.com</span>
				</a>
				<a href="https://example.com/contact" target="_blank" rel="noreferrer" aria-label="Open contact form">
					<img src={contactChatIcon} alt="" aria-hidden="true" />
					<span>Contact form</span>
				</a>
			</div>
		</footer>
	)
}

export function PageLayout({ children }) {
	return (
		<div className="site-shell">
			<Header />
			<main className="site-main">{children}</main>
			<Footer />
		</div>
	)
}
