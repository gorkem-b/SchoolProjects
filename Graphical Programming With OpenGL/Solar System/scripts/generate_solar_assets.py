from __future__ import annotations

import math
import os
import random
import ssl
import urllib.request
from pathlib import Path

from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageOps


ROOT = Path(__file__).resolve().parents[1] / "res" / "textures"

DOWNLOADS = {
    "sun.jpg": "https://www.solarsystemscope.com/textures/download/2k_sun.jpg",
    "mercury.jpg": "https://www.solarsystemscope.com/textures/download/2k_mercury.jpg",
    "venus.jpg": "https://www.solarsystemscope.com/textures/download/2k_venus_surface.jpg",
    "moon.jpg": "https://www.solarsystemscope.com/textures/download/2k_moon.jpg",
    "mars.jpg": "https://www.solarsystemscope.com/textures/download/2k_mars.jpg",
    "jupiter.jpg": "https://www.solarsystemscope.com/textures/download/2k_jupiter.jpg",
    "saturn.jpg": "https://www.solarsystemscope.com/textures/download/2k_saturn.jpg",
    "uranus.jpg": "https://www.solarsystemscope.com/textures/download/2k_uranus.jpg",
    "neptune.jpg": "https://www.solarsystemscope.com/textures/download/2k_neptune.jpg",
}


def ensure_directory() -> None:
    ROOT.mkdir(parents=True, exist_ok=True)


def download_file(name: str, url: str) -> None:
    destination = ROOT / name
    temporary = destination.with_suffix(destination.suffix + ".download")

    try:
        print(f"Downloading {name}...")
        urllib.request.urlretrieve(url, temporary)
        os.replace(temporary, destination)
        print(f"  saved -> {destination}")
    except Exception as exc:  # noqa: BLE001 - script should continue if a single download fails
        if temporary.exists():
            temporary.unlink()
        print(f"  skipped ({exc})")


def random_layer(size: tuple[int, int], density: int, seed: int) -> Image.Image:
    rng = random.Random(seed)
    reduced = (max(4, size[0] // density), max(4, size[1] // density))
    values = bytes(rng.randrange(256) for _ in range(reduced[0] * reduced[1]))
    image = Image.frombytes("L", reduced, values)
    return image.resize(size, Image.Resampling.BICUBIC)


def layered_noise(size: tuple[int, int], seed: int) -> Image.Image:
    image = random_layer(size, 32, seed + 11)
    for density, alpha in ((20, 0.55), (12, 0.35), (6, 0.2), (3, 0.1)):
        image = Image.blend(image, random_layer(size, density, seed + density * 13), alpha)
    return ImageOps.autocontrast(image)


def save_rgb(image: Image.Image, path: Path) -> None:
    image.convert("RGB").save(path, quality=95)
    print(f"Generated {path.name}")


def generate_rock_texture(name: str, colors: tuple[str, str, str], seed: int, craters: int) -> None:
    width, height = 2048, 1024
    base = layered_noise((width, height), seed)
    detail = layered_noise((width, height), seed + 71)
    roughness = ImageChops.multiply(base, detail)
    roughness = Image.blend(base, roughness, 0.45)
    roughness = ImageOps.autocontrast(roughness)
    colored = ImageOps.colorize(roughness, black=colors[0], mid=colors[1], white=colors[2])

    overlay = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(overlay)
    rng = random.Random(seed * 17)

    for _ in range(craters):
        radius = rng.randint(5, 28)
        x = rng.randint(-radius, width + radius)
        y = rng.randint(-radius, height + radius)
        ring_alpha = rng.randint(16, 42)
        fill_alpha = rng.randint(10, 30)
        draw.ellipse((x - radius, y - radius, x + radius, y + radius), outline=(18, 14, 12, ring_alpha), width=2)
        draw.ellipse(
            (x - radius * 0.6, y - radius * 0.6, x + radius * 0.6, y + radius * 0.6),
            fill=(255, 245, 235, fill_alpha),
        )

    composed = Image.alpha_composite(colored.convert("RGBA"), overlay).filter(ImageFilter.GaussianBlur(radius=0.35))
    save_rgb(composed, ROOT / name)


def generate_corona_texture() -> None:
    width, height = 2048, 1024
    plasma = layered_noise((width, height), 901)
    wisps = layered_noise((width, height), 1207)
    texture = Image.blend(plasma, wisps.filter(ImageFilter.GaussianBlur(radius=5.0)), 0.55)
    texture = ImageOps.colorize(texture, black="#4d1500", mid="#ff7600", white="#fff1a8")

    alpha = Image.new("L", (width, height), 0)
    draw = ImageDraw.Draw(alpha)
    rng = random.Random(77)
    for _ in range(320):
        x = rng.randint(0, width - 1)
        y = rng.randint(0, height - 1)
        radius_x = rng.randint(40, 180)
        radius_y = rng.randint(14, 84)
        strength = rng.randint(18, 54)
        draw.ellipse((x - radius_x, y - radius_y, x + radius_x, y + radius_y), fill=strength)

    alpha = alpha.filter(ImageFilter.GaussianBlur(radius=20.0))
    alpha = Image.blend(alpha, plasma, 0.5)
    alpha = ImageOps.autocontrast(alpha)
    alpha = alpha.point(lambda value: max(10, min(255, int(value * 0.78))))

    result = texture.convert("RGBA")
    result.putalpha(alpha)
    result.save(ROOT / "sun_corona.png")
    print("Generated sun_corona.png")


def generate_dust_ring() -> None:
    width, height = 2048, 256
    noise = layered_noise((width, height), 451)
    image = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    pixels = image.load()
    noise_pixels = noise.load()

    for y in range(height):
        band = 0.5 + 0.32 * math.sin((y / height) * math.pi * 4.8) + 0.18 * math.cos((y / height) * math.pi * 9.5)
        band = max(0.0, min(1.0, band))
        width_falloff = 1.0 - abs((y / max(1, height - 1)) * 2.0 - 1.0)
        width_falloff = max(0.0, width_falloff) ** 0.75

        for x in range(width):
            grain = noise_pixels[x, y] / 255.0
            sparkle = 1.0 if (x * 17 + y * 29) % 257 == 0 else 0.0
            alpha = int(max(0.0, min(1.0, (0.14 + grain * 0.48 + sparkle * 0.35) * band * width_falloff)) * 255)
            brightness = int(70 + 130 * grain + 40 * band + 30 * sparkle)
            pixels[x, y] = (brightness, int(brightness * 0.88), int(brightness * 0.72), alpha)

    image = image.filter(ImageFilter.GaussianBlur(radius=0.6))
    image.save(ROOT / "dust_ring.png")
    print("Generated dust_ring.png")


def main() -> None:
    ensure_directory()
    ssl._create_default_https_context = ssl._create_unverified_context

    for name, url in DOWNLOADS.items():
        download_file(name, url)

    generate_corona_texture()
    generate_dust_ring()
    generate_rock_texture("asteroid.jpg", ("#2b2521", "#806d5a", "#d2c0a7"), 101, 220)
    generate_rock_texture("icy_moon.jpg", ("#293747", "#9cb9ca", "#eef7ff"), 207, 150)
    generate_rock_texture("volcanic_moon.jpg", ("#38160f", "#a15f20", "#f1dc9c"), 303, 260)
    generate_rock_texture("dark_moon.jpg", ("#161719", "#55514b", "#b7aa98"), 409, 180)
    generate_rock_texture("hazy_moon.jpg", ("#2d2419", "#998364", "#d8c59d"), 515, 140)
    generate_rock_texture("pluto.jpg", ("#1f1e21", "#8f7767", "#d8d1c8"), 611, 190)


if __name__ == "__main__":
    main()