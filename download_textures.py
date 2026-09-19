import urllib.request
import os

textures = {
    "sun.jpg": "https://www.solarsystemscope.com/textures/download/2k_sun.jpg",
    "mercury.jpg": "https://www.solarsystemscope.com/textures/download/2k_mercury.jpg",
    "venus.jpg": "https://www.solarsystemscope.com/textures/download/2k_venus_surface.jpg",
    "earth.jpg": "https://www.solarsystemscope.com/textures/download/2k_earth_daymap.jpg",
    "moon.jpg": "https://www.solarsystemscope.com/textures/download/2k_moon.jpg",
    "mars.jpg": "https://www.solarsystemscope.com/textures/download/2k_mars.jpg",
    "jupiter.jpg": "https://www.solarsystemscope.com/textures/download/2k_jupiter.jpg",
    "saturn.jpg": "https://www.solarsystemscope.com/textures/download/2k_saturn.jpg",
    "uranus.jpg": "https://www.solarsystemscope.com/textures/download/2k_uranus.jpg",
    "neptune.jpg": "https://www.solarsystemscope.com/textures/download/2k_neptune.jpg",
    "earth_clouds.png": "https://upload.wikimedia.org/wikipedia/commons/e/e5/NASA_Earth_Cloud_Map.jpg", # solarsystemscope uses jpg for clouds, we need png if we want transparency but let's just grab jpg and save as png to let STB handle it, or we can use the jpg directly. Wait, in SolarSystem.c we look for earth_clouds.png
}

os.makedirs("textures", exist_ok=True)

for name, url in textures.items():
    path = os.path.join("textures", name)
    if not os.path.exists(path):
        print(f"Downloading {name}...")
        try:
            req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
            with urllib.request.urlopen(req) as response, open(path, 'wb') as out_file:
                out_file.write(response.read())
            print(f"Saved {path}")
        except Exception as e:
            print(f"Failed to download {name}: {e}")
    else:
        print(f"Already have {name}")

# For rings, let's create simple 1x256 procedural gradients using PIL if needed, 
# or just download saturn ring.
try:
    req = urllib.request.Request("https://www.solarsystemscope.com/textures/download/2k_saturn_ring_alpha.png", headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req) as response, open("textures/saturn_ring.png", 'wb') as out_file:
        out_file.write(response.read())
except Exception as e:
    pass
