from PIL import Image, ImageDraw, ImageFont
import math
import os

size = 1024
bg = (15, 20, 35)
img = Image.new("RGBA", (size, size), bg)
draw = ImageDraw.Draw(img)

# Concentric rings
for i, alpha in [(380, 40), (330, 60), (280, 80)]:
    bbox = [size / 2 - i, size / 2 - i, size / 2 + i, size / 2 + i]
    draw.ellipse(bbox, outline=(80, 140, 255, alpha), width=10)

# Sine wave
wave_color = (120, 200, 255, 255)
pts = []
amp = 120
for x in range(120, size - 120, 4):
    t = (x - 120) / (size - 240) * 2 * math.pi * 3
    y = size / 2 + math.sin(t) * amp
    pts.append((x, y))

draw.line(pts, fill=wave_color, width=16, joint="curve")

# Text
text = "40Hz"
font = None
for path in [
    "/System/Library/Fonts/Supplemental/Helvetica.ttf",
    "/System/Library/Fonts/Supplemental/Arial.ttf",
    "/System/Library/Fonts/Supplemental/Verdana.ttf",
]:
    if os.path.exists(path):
        font = ImageFont.truetype(path, 180)
        break
if font is None:
    font = ImageFont.load_default()

bbox = draw.textbbox((0, 0), text, font=font)
text_w = bbox[2] - bbox[0]
text_h = bbox[3] - bbox[1]
text_pos = ((size - text_w) / 2, size * 0.68 - text_h / 2)

# Soft glow behind text
for r in range(8, 0, -1):
    draw.text(text_pos, text, font=font, fill=(120, 200, 255, 18 * r))

draw.text(text_pos, text, font=font, fill=(235, 245, 255, 255))

out_path = "/Users/user/Projects/PNASSound/Assets/AppIcon-1024.png"
os.makedirs(os.path.dirname(out_path), exist_ok=True)
img.save(out_path)
print(out_path)
