"""Generate Embermere's project-owned starter UI icon family.

The icons use only Python's standard library so the source art is deterministic
on every development machine. Output PNGs are 128x128 RGBA images with a shared
charcoal, moss, stone, gold, and ember visual language.
"""

from __future__ import annotations

import math
import struct
import zlib
from pathlib import Path


OUTPUT_DIR = Path(__file__).resolve().parents[1] / "ArtSource" / "UI" / "Icons"
ICON_SIZE = 128
SUPERSAMPLE = 3


Color = tuple[int, int, int, int]


CHARCOAL: Color = (18, 27, 23, 255)
MOSS_DARK: Color = (35, 55, 40, 255)
MOSS: Color = (83, 126, 70, 255)
STONE: Color = (151, 157, 139, 255)
STONE_LIGHT: Color = (207, 208, 187, 255)
GOLD: Color = (232, 172, 61, 255)
GOLD_LIGHT: Color = (255, 219, 116, 255)
EMBER: Color = (243, 91, 39, 255)
EMBER_LIGHT: Color = (255, 167, 70, 255)
IRON: Color = (91, 102, 99, 255)
IRON_LIGHT: Color = (174, 184, 177, 255)
LEATHER: Color = (130, 77, 42, 255)
LEATHER_LIGHT: Color = (190, 126, 66, 255)
TONIC: Color = (82, 185, 103, 255)
TONIC_LIGHT: Color = (164, 238, 148, 255)
VOID: Color = (0, 0, 0, 0)


class Canvas:
    def __init__(self) -> None:
        self.width = ICON_SIZE * SUPERSAMPLE
        self.height = ICON_SIZE * SUPERSAMPLE
        self.pixels = bytearray(self.width * self.height * 4)

    @staticmethod
    def _s(value: float) -> int:
        return int(round(value * SUPERSAMPLE))

    def blend(self, x: int, y: int, color: Color) -> None:
        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return
        index = (y * self.width + x) * 4
        alpha = color[3] / 255.0
        inverse = 1.0 - alpha
        self.pixels[index] = int(color[0] * alpha + self.pixels[index] * inverse)
        self.pixels[index + 1] = int(color[1] * alpha + self.pixels[index + 1] * inverse)
        self.pixels[index + 2] = int(color[2] * alpha + self.pixels[index + 2] * inverse)
        self.pixels[index + 3] = min(255, int(color[3] + self.pixels[index + 3] * inverse))

    def rect(self, x0: float, y0: float, x1: float, y1: float, color: Color) -> None:
        for y in range(self._s(y0), self._s(y1)):
            for x in range(self._s(x0), self._s(x1)):
                self.blend(x, y, color)

    def rounded_rect(
        self,
        x0: float,
        y0: float,
        x1: float,
        y1: float,
        radius: float,
        color: Color,
    ) -> None:
        sx0, sy0, sx1, sy1 = map(self._s, (x0, y0, x1, y1))
        sr = self._s(radius)
        for y in range(sy0, sy1):
            for x in range(sx0, sx1):
                cx = min(max(x, sx0 + sr), sx1 - sr - 1)
                cy = min(max(y, sy0 + sr), sy1 - sr - 1)
                if (x - cx) ** 2 + (y - cy) ** 2 <= sr * sr:
                    self.blend(x, y, color)

    def ellipse(self, cx: float, cy: float, rx: float, ry: float, color: Color) -> None:
        scx, scy, srx, sry = map(self._s, (cx, cy, rx, ry))
        if srx <= 0 or sry <= 0:
            return
        for y in range(scy - sry, scy + sry + 1):
            normalized_y = ((y - scy) / sry) ** 2
            if normalized_y > 1.0:
                continue
            span = int(srx * math.sqrt(1.0 - normalized_y))
            for x in range(scx - span, scx + span + 1):
                self.blend(x, y, color)

    def polygon(self, points: list[tuple[float, float]], color: Color) -> None:
        scaled = [(self._s(x), self._s(y)) for x, y in points]
        min_y = max(0, min(point[1] for point in scaled))
        max_y = min(self.height - 1, max(point[1] for point in scaled))
        count = len(scaled)
        for y in range(min_y, max_y + 1):
            intersections: list[float] = []
            for index in range(count):
                x1, y1 = scaled[index]
                x2, y2 = scaled[(index + 1) % count]
                if y1 == y2 or y < min(y1, y2) or y >= max(y1, y2):
                    continue
                intersections.append(x1 + (y - y1) * (x2 - x1) / (y2 - y1))
            intersections.sort()
            for start in range(0, len(intersections) - 1, 2):
                for x in range(max(0, int(math.ceil(intersections[start]))), min(self.width, int(intersections[start + 1]) + 1)):
                    self.blend(x, y, color)

    def line(
        self,
        points: list[tuple[float, float]],
        color: Color,
        width: float,
    ) -> None:
        radius = max(1, self._s(width) // 2)
        for start, end in zip(points, points[1:]):
            x0, y0 = self._s(start[0]), self._s(start[1])
            x1, y1 = self._s(end[0]), self._s(end[1])
            steps = max(abs(x1 - x0), abs(y1 - y0), 1)
            for step in range(steps + 1):
                x = int(round(x0 + (x1 - x0) * step / steps))
                y = int(round(y0 + (y1 - y0) * step / steps))
                for py in range(y - radius, y + radius + 1):
                    for px in range(x - radius, x + radius + 1):
                        if (px - x) ** 2 + (py - y) ** 2 <= radius * radius:
                            self.blend(px, py, color)

    def frame(self, accent: Color) -> None:
        self.rounded_rect(3, 3, 125, 125, 10, (5, 8, 7, 235))
        self.rounded_rect(6, 6, 122, 122, 8, accent)
        self.rounded_rect(9, 9, 119, 119, 6, CHARCOAL)
        self.polygon([(12, 93), (43, 116), (12, 116)], (42, 65, 46, 180))
        self.polygon([(116, 14), (116, 48), (78, 14)], (255, 255, 255, 10))
        self.ellipse(64, 64, 46, 46, (63, 83, 64, 75))

    def save(self, path: Path) -> None:
        output = bytearray(ICON_SIZE * ICON_SIZE * 4)
        samples = SUPERSAMPLE * SUPERSAMPLE
        for y in range(ICON_SIZE):
            for x in range(ICON_SIZE):
                totals = [0, 0, 0, 0]
                for sy in range(SUPERSAMPLE):
                    for sx in range(SUPERSAMPLE):
                        source = (((y * SUPERSAMPLE + sy) * self.width) + x * SUPERSAMPLE + sx) * 4
                        for channel in range(4):
                            totals[channel] += self.pixels[source + channel]
                destination = (y * ICON_SIZE + x) * 4
                for channel in range(4):
                    output[destination + channel] = totals[channel] // samples

        raw = bytearray()
        row_bytes = ICON_SIZE * 4
        for y in range(ICON_SIZE):
            raw.append(0)
            raw.extend(output[y * row_bytes : (y + 1) * row_bytes])

        def chunk(kind: bytes, payload: bytes) -> bytes:
            return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF)

        png = bytearray(b"\x89PNG\r\n\x1a\n")
        png.extend(chunk(b"IHDR", struct.pack(">IIBBBBB", ICON_SIZE, ICON_SIZE, 8, 6, 0, 0, 0)))
        png.extend(chunk(b"IDAT", zlib.compress(bytes(raw), 9)))
        png.extend(chunk(b"IEND", b""))
        path.write_bytes(png)


def ember_rune(canvas: Canvas, cx: float, cy: float, scale: float = 1.0) -> None:
    canvas.line(
        [(cx, cy - 16 * scale), (cx - 8 * scale, cy - 4 * scale), (cx + 3 * scale, cy + 2 * scale),
         (cx - 7 * scale, cy + 17 * scale), (cx + 10 * scale, cy + 5 * scale)],
        EMBER_LIGHT,
        3.5 * scale,
    )
    canvas.ellipse(cx, cy + 2 * scale, 3.5 * scale, 3.5 * scale, GOLD_LIGHT)


def draw_recruit_pack(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.rounded_rect(33, 34, 95, 103, 10, LEATHER)
    canvas.rounded_rect(39, 28, 89, 55, 10, LEATHER_LIGHT)
    canvas.line([(39, 47), (89, 47)], GOLD, 4)
    canvas.rounded_rect(46, 63, 82, 89, 5, (99, 57, 34, 255))
    canvas.line([(33, 47), (25, 65), (28, 96)], IRON_LIGHT, 5)
    canvas.line([(95, 47), (103, 65), (100, 96)], IRON_LIGHT, 5)
    canvas.ellipse(64, 58, 8, 8, GOLD)
    ember_rune(canvas, 64, 58, 0.42)


def draw_marsh_tonic(canvas: Canvas) -> None:
    canvas.frame(TONIC)
    canvas.rounded_rect(50, 25, 78, 40, 4, LEATHER_LIGHT)
    canvas.rounded_rect(46, 37, 82, 99, 11, IRON_LIGHT)
    canvas.rounded_rect(50, 48, 78, 95, 8, (42, 92, 62, 255))
    canvas.polygon([(50, 69), (78, 60), (78, 95), (50, 95)], TONIC)
    canvas.ellipse(61, 72, 6, 9, TONIC_LIGHT)
    canvas.ellipse(71, 82, 4, 6, (204, 255, 182, 210))
    canvas.line([(45, 100), (83, 100)], GOLD, 4)


def draw_sword(canvas: Canvas) -> None:
    canvas.frame(EMBER)
    canvas.polygon([(70, 22), (80, 28), (61, 78), (51, 72)], IRON_LIGHT)
    canvas.polygon([(70, 22), (80, 28), (75, 42)], STONE_LIGHT)
    canvas.line([(44, 72), (69, 86)], GOLD, 7)
    canvas.line([(58, 81), (43, 105)], LEATHER_LIGHT, 8)
    canvas.ellipse(41, 108, 7, 5, EMBER)


def draw_shield(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.polygon([(64, 24), (99, 39), (92, 82), (64, 105), (36, 82), (29, 39)], IRON)
    canvas.polygon([(64, 31), (91, 42), (85, 76), (64, 95)], (117, 128, 119, 255))
    canvas.line([(64, 30), (64, 95)], GOLD, 4)
    ember_rune(canvas, 60, 62, 0.72)


def draw_helmet(canvas: Canvas) -> None:
    canvas.frame(IRON_LIGHT)
    canvas.ellipse(64, 58, 36, 34, IRON)
    canvas.rect(29, 56, 99, 76, IRON)
    canvas.polygon([(38, 72), (52, 72), (48, 101), (32, 94)], IRON_LIGHT)
    canvas.polygon([(76, 72), (90, 72), (96, 94), (80, 101)], IRON_LIGHT)
    canvas.polygon([(59, 32), (69, 32), (75, 76), (53, 76)], STONE_LIGHT)
    canvas.line([(28, 76), (100, 76)], GOLD, 5)
    canvas.ellipse(64, 76, 5, 5, EMBER)


def draw_breastplate(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.polygon([(44, 27), (56, 36), (72, 36), (84, 27), (98, 45), (89, 101), (39, 101), (30, 45)], IRON)
    canvas.polygon([(48, 38), (64, 47), (80, 38), (84, 87), (64, 98), (44, 87)], IRON_LIGHT)
    canvas.line([(40, 67), (88, 67)], GOLD, 4)
    ember_rune(canvas, 64, 63, 0.68)


def draw_gauntlet(canvas: Canvas) -> None:
    canvas.frame(IRON_LIGHT)
    canvas.rounded_rect(42, 54, 84, 102, 9, IRON)
    for index, x in enumerate((43, 53, 63, 73)):
        canvas.rounded_rect(x, 27 + index * 2, x + 10, 68, 5, IRON_LIGHT)
    canvas.polygon([(42, 65), (28, 54), (24, 67), (45, 88)], IRON_LIGHT)
    canvas.line([(42, 76), (84, 76)], GOLD, 5)
    canvas.ellipse(64, 78, 5, 5, EMBER)


def draw_leggings(canvas: Canvas) -> None:
    canvas.frame(MOSS)
    canvas.polygon([(39, 26), (89, 26), (84, 64), (76, 105), (56, 105), (61, 62), (51, 105), (31, 105), (39, 64)], LEATHER)
    canvas.rect(39, 27, 89, 43, LEATHER_LIGHT)
    canvas.line([(39, 44), (89, 44)], GOLD, 4)
    canvas.polygon([(55, 48), (64, 58), (73, 48), (69, 76), (59, 76)], IRON)


def draw_boot(canvas: Canvas) -> None:
    canvas.frame(LEATHER_LIGHT)
    canvas.polygon([(48, 27), (79, 27), (81, 75), (103, 89), (99, 104), (43, 104), (34, 94), (48, 78)], LEATHER)
    canvas.line([(48, 44), (79, 44)], GOLD, 5)
    canvas.line([(47, 62), (80, 62)], IRON_LIGHT, 4)
    canvas.polygon([(43, 97), (99, 97), (95, 108), (47, 108)], IRON)


def draw_cloak(canvas: Canvas) -> None:
    canvas.frame(MOSS)
    canvas.ellipse(64, 31, 16, 9, GOLD)
    canvas.polygon([(51, 34), (77, 34), (95, 104), (64, 92), (33, 104)], (72, 116, 72, 255))
    canvas.polygon([(64, 38), (76, 98), (64, 92)], MOSS)
    canvas.line([(48, 39), (80, 39)], GOLD_LIGHT, 4)
    ember_rune(canvas, 64, 64, 0.62)


def draw_pendant(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.line([(35, 31), (45, 56), (64, 72), (83, 56), (93, 31)], GOLD_LIGHT, 5)
    canvas.ellipse(64, 79, 22, 25, IRON)
    canvas.ellipse(64, 79, 15, 18, MOSS_DARK)
    ember_rune(canvas, 64, 78, 0.62)


def draw_ring(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.ellipse(64, 73, 35, 35, GOLD)
    canvas.ellipse(64, 73, 23, 23, CHARCOAL)
    canvas.polygon([(49, 40), (57, 25), (71, 25), (79, 40), (69, 52), (59, 52)], EMBER)
    canvas.polygon([(57, 27), (71, 27), (75, 39), (64, 46), (53, 39)], EMBER_LIGHT)


def draw_missing_item(canvas: Canvas) -> None:
    canvas.frame(STONE)
    canvas.rounded_rect(35, 35, 93, 98, 10, MOSS_DARK)
    canvas.line([(47, 57), (64, 43), (81, 57), (64, 71), (64, 86)], STONE_LIGHT, 6)
    canvas.ellipse(64, 96, 5, 5, GOLD)


def draw_missing_slot(canvas: Canvas) -> None:
    canvas.frame(IRON)
    canvas.polygon([(64, 25), (96, 43), (88, 92), (64, 105), (40, 92), (32, 43)], (45, 58, 49, 255))
    canvas.line([(47, 49), (81, 83), (81, 49), (47, 83)], STONE, 6)
    canvas.ellipse(64, 66, 5, 5, GOLD)


ICON_BUILDERS = {
    "T_Icon_Item_RecruitPack.png": draw_recruit_pack,
    "T_Icon_Item_MarshTonic.png": draw_marsh_tonic,
    "T_Icon_Slot_MainHand.png": draw_sword,
    "T_Icon_Slot_OffHand.png": draw_shield,
    "T_Icon_Slot_Head.png": draw_helmet,
    "T_Icon_Slot_Chest.png": draw_breastplate,
    "T_Icon_Slot_Hands.png": draw_gauntlet,
    "T_Icon_Slot_Legs.png": draw_leggings,
    "T_Icon_Slot_Feet.png": draw_boot,
    "T_Icon_Slot_Back.png": draw_cloak,
    "T_Icon_Slot_Neck.png": draw_pendant,
    "T_Icon_Slot_Ring.png": draw_ring,
    "T_Icon_Item_Missing.png": draw_missing_item,
    "T_Icon_Slot_Missing.png": draw_missing_slot,
}


def main() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    for filename, builder in ICON_BUILDERS.items():
        canvas = Canvas()
        builder(canvas)
        canvas.save(OUTPUT_DIR / filename)
    print(f"Generated {len(ICON_BUILDERS)} Embermere UI icons in {OUTPUT_DIR}")


if __name__ == "__main__":
    main()
