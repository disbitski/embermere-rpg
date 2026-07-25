"""Generate Embermere's project-owned starter UI art family.

The art uses only Python's standard library so the source is deterministic on
every development machine. Icons are 128x128 RGBA images and the equipment
paper-doll backdrop is 128x160, all using Embermere's shared charcoal, moss,
stone, gold, and ember visual language.
"""

from __future__ import annotations

import math
import struct
import zlib
from pathlib import Path


OUTPUT_DIR = Path(__file__).resolve().parents[1] / "ArtSource" / "UI" / "Icons"
ICON_SIZE = 128
PAPER_DOLL_WIDTH = 128
PAPER_DOLL_HEIGHT = 160
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
HOLY: Color = (246, 211, 108, 255)
HOLY_LIGHT: Color = (255, 244, 194, 255)
RANGER: Color = (78, 151, 89, 255)
RANGER_LIGHT: Color = (166, 226, 132, 255)
FROST: Color = (71, 165, 220, 255)
FROST_LIGHT: Color = (174, 230, 255, 255)
ARCANE: Color = (117, 89, 211, 255)
ARCANE_LIGHT: Color = (202, 178, 255, 255)
VOID: Color = (0, 0, 0, 0)


class Canvas:
    def __init__(self, output_width: int = ICON_SIZE, output_height: int = ICON_SIZE) -> None:
        self.output_width = output_width
        self.output_height = output_height
        self.width = output_width * SUPERSAMPLE
        self.height = output_height * SUPERSAMPLE
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
        output = bytearray(self.output_width * self.output_height * 4)
        samples = SUPERSAMPLE * SUPERSAMPLE
        for y in range(self.output_height):
            for x in range(self.output_width):
                totals = [0, 0, 0, 0]
                for sy in range(SUPERSAMPLE):
                    for sx in range(SUPERSAMPLE):
                        source = (((y * SUPERSAMPLE + sy) * self.width) + x * SUPERSAMPLE + sx) * 4
                        for channel in range(4):
                            totals[channel] += self.pixels[source + channel]
                destination = (y * self.output_width + x) * 4
                for channel in range(4):
                    output[destination + channel] = totals[channel] // samples

        raw = bytearray()
        row_bytes = self.output_width * 4
        for y in range(self.output_height):
            raw.append(0)
            raw.extend(output[y * row_bytes : (y + 1) * row_bytes])

        def chunk(kind: bytes, payload: bytes) -> bytes:
            return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF)

        png = bytearray(b"\x89PNG\r\n\x1a\n")
        png.extend(
            chunk(
                b"IHDR",
                struct.pack(
                    ">IIBBBBB",
                    self.output_width,
                    self.output_height,
                    8,
                    6,
                    0,
                    0,
                    0,
                ),
            )
        )
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


def impact_star(
    canvas: Canvas,
    cx: float,
    cy: float,
    outer: float,
    inner: float,
    color: Color,
) -> None:
    points: list[tuple[float, float]] = []
    for index in range(16):
        radius = outer if index % 2 == 0 else inner
        angle = -math.pi / 2 + index * math.pi / 8
        points.append((cx + math.cos(angle) * radius, cy + math.sin(angle) * radius))
    canvas.polygon(points, color)


def leaf(
    canvas: Canvas,
    cx: float,
    cy: float,
    width: float,
    height: float,
    color: Color,
) -> None:
    canvas.polygon(
        [
            (cx, cy - height / 2),
            (cx + width / 2, cy),
            (cx, cy + height / 2),
            (cx - width / 2, cy),
        ],
        color,
    )
    canvas.line([(cx, cy - height / 3), (cx, cy + height / 3)], MOSS_DARK, 2.5)


def draw_ability_strike(canvas: Canvas) -> None:
    canvas.frame(EMBER)
    canvas.polygon([(79, 23), (88, 31), (56, 82), (47, 73)], IRON_LIGHT)
    canvas.polygon([(79, 23), (88, 31), (80, 44)], STONE_LIGHT)
    canvas.line([(40, 70), (66, 88)], GOLD, 6)
    canvas.line([(56, 84), (39, 105)], LEATHER_LIGHT, 8)
    canvas.line([(25, 42), (56, 31), (91, 42)], EMBER_LIGHT, 4)
    canvas.line([(29, 52), (53, 45)], GOLD_LIGHT, 3)


def draw_ability_taunt(canvas: Canvas) -> None:
    canvas.frame(EMBER)
    canvas.polygon([(40, 38), (61, 29), (82, 38), (87, 77), (64, 99), (39, 78)], IRON)
    canvas.polygon([(46, 45), (61, 38), (78, 45), (76, 69), (63, 82), (47, 69)], LEATHER)
    canvas.ellipse(56, 58, 5, 4, EMBER_LIGHT)
    canvas.ellipse(70, 58, 5, 4, EMBER_LIGHT)
    canvas.line([(24, 49), (34, 58), (23, 66)], GOLD_LIGHT, 4)
    canvas.line([(103, 49), (93, 58), (104, 66)], GOLD_LIGHT, 4)
    canvas.line([(25, 82), (39, 76)], EMBER, 4)
    canvas.line([(102, 82), (87, 76)], EMBER, 4)


def draw_ability_shield_slam(canvas: Canvas) -> None:
    canvas.frame(GOLD)
    canvas.polygon([(54, 25), (84, 38), (79, 78), (54, 101), (29, 78), (24, 38)], IRON)
    canvas.polygon([(54, 32), (76, 42), (72, 71), (54, 90)], IRON_LIGHT)
    canvas.line([(54, 33), (54, 90)], GOLD, 4)
    impact_star(canvas, 87, 60, 24, 9, EMBER_LIGHT)
    canvas.ellipse(87, 60, 7, 7, HOLY_LIGHT)


def draw_ability_battle_shout(canvas: Canvas) -> None:
    canvas.frame(EMBER)
    canvas.polygon([(29, 55), (48, 46), (75, 29), (78, 77), (50, 67)], LEATHER_LIGHT)
    canvas.polygon([(50, 47), (70, 36), (70, 68), (50, 62)], GOLD)
    canvas.line([(34, 67), (25, 86)], IRON_LIGHT, 7)
    canvas.line([(84, 42), (101, 31)], HOLY_LIGHT, 4)
    canvas.line([(87, 57), (108, 57)], HOLY_LIGHT, 5)
    canvas.line([(84, 72), (101, 84)], HOLY_LIGHT, 4)
    canvas.ellipse(105, 57, 4, 4, EMBER_LIGHT)


def draw_ability_smite(canvas: Canvas) -> None:
    canvas.frame(HOLY)
    canvas.polygon([(58, 20), (76, 20), (70, 57), (57, 57)], HOLY_LIGHT)
    impact_star(canvas, 62, 71, 35, 13, GOLD_LIGHT)
    canvas.polygon([(54, 54), (72, 54), (65, 102), (48, 102)], STONE_LIGHT)
    canvas.ellipse(62, 71, 8, 8, EMBER_LIGHT)


def draw_ability_lesser_heal(canvas: Canvas) -> None:
    canvas.frame(HOLY)
    canvas.ellipse(49, 52, 20, 19, EMBER)
    canvas.ellipse(79, 52, 20, 19, EMBER)
    canvas.polygon([(31, 55), (97, 55), (64, 102)], EMBER)
    canvas.rect(58, 38, 70, 82, HOLY_LIGHT)
    canvas.rect(43, 53, 85, 67, HOLY_LIGHT)
    canvas.ellipse(64, 60, 8, 8, GOLD_LIGHT)


def draw_ability_ward(canvas: Canvas) -> None:
    canvas.frame(HOLY)
    canvas.ellipse(64, 62, 43, 43, (111, 91, 39, 255))
    canvas.ellipse(64, 62, 36, 36, CHARCOAL)
    canvas.polygon([(64, 28), (92, 40), (87, 76), (64, 98), (41, 76), (36, 40)], IRON_LIGHT)
    canvas.polygon([(64, 35), (84, 44), (80, 70), (64, 87)], HOLY)
    ember_rune(canvas, 61, 61, 0.55)


def draw_ability_judgment(canvas: Canvas) -> None:
    canvas.frame(HOLY)
    canvas.line([(64, 29), (64, 91)], GOLD_LIGHT, 6)
    canvas.line([(34, 48), (94, 48)], GOLD, 6)
    canvas.line([(42, 49), (31, 73), (52, 73), (42, 49)], HOLY_LIGHT, 3)
    canvas.line([(86, 49), (75, 73), (97, 73), (86, 49)], HOLY_LIGHT, 3)
    canvas.line([(29, 74), (54, 74)], EMBER_LIGHT, 5)
    canvas.line([(73, 74), (99, 74)], EMBER_LIGHT, 5)
    impact_star(canvas, 64, 94, 17, 7, HOLY_LIGHT)


def draw_ability_quick_shot(canvas: Canvas) -> None:
    canvas.frame(RANGER)
    canvas.line([(43, 27), (30, 47), (28, 71), (42, 98)], LEATHER_LIGHT, 6)
    canvas.line([(43, 27), (42, 98)], GOLD_LIGHT, 3)
    canvas.line([(26, 64), (101, 51)], IRON_LIGHT, 5)
    canvas.polygon([(102, 51), (87, 43), (90, 58)], STONE_LIGHT)
    canvas.line([(34, 62), (27, 55)], RANGER_LIGHT, 3)
    canvas.line([(34, 62), (30, 71)], RANGER_LIGHT, 3)


def draw_ability_snare(canvas: Canvas) -> None:
    canvas.frame(RANGER)
    canvas.polygon([(51, 27), (76, 27), (77, 72), (96, 86), (92, 101), (42, 101), (34, 91), (51, 73)], LEATHER)
    canvas.line([(49, 47), (78, 47)], GOLD, 4)
    canvas.line([(31, 54), (91, 82), (38, 91), (88, 57)], RANGER_LIGHT, 6)
    leaf(canvas, 31, 54, 15, 25, TONIC)
    leaf(canvas, 91, 82, 15, 25, TONIC)
    canvas.ellipse(63, 71, 8, 8, MOSS_DARK)


def draw_ability_twin_cut(canvas: Canvas) -> None:
    canvas.frame(RANGER)
    canvas.polygon([(37, 26), (46, 24), (73, 83), (63, 87)], IRON_LIGHT)
    canvas.polygon([(91, 25), (99, 34), (54, 85), (46, 77)], IRON_LIGHT)
    canvas.line([(57, 78), (39, 99)], LEATHER_LIGHT, 7)
    canvas.line([(69, 79), (88, 101)], LEATHER_LIGHT, 7)
    canvas.line([(31, 44), (52, 35)], RANGER_LIGHT, 4)
    canvas.line([(77, 38), (101, 49)], GOLD_LIGHT, 4)


def draw_ability_natures_focus(canvas: Canvas) -> None:
    canvas.frame(RANGER)
    leaf(canvas, 64, 60, 70, 82, RANGER_LIGHT)
    canvas.ellipse(64, 60, 28, 17, MOSS_DARK)
    canvas.ellipse(64, 60, 17, 11, STONE_LIGHT)
    canvas.ellipse(64, 60, 7, 7, GOLD_LIGHT)
    canvas.line([(64, 86), (64, 105)], TONIC_LIGHT, 4)
    canvas.line([(64, 96), (52, 87)], TONIC_LIGHT, 3)
    canvas.line([(64, 97), (76, 88)], TONIC_LIGHT, 3)


def draw_ability_spark_bolt(canvas: Canvas) -> None:
    canvas.frame(FROST)
    canvas.polygon([(72, 18), (41, 65), (60, 65), (48, 108), (91, 53), (70, 53)], HOLY_LIGHT)
    canvas.polygon([(68, 25), (49, 58), (67, 58), (58, 88), (82, 57), (65, 57)], FROST_LIGHT)
    canvas.ellipse(91, 39, 8, 8, ARCANE_LIGHT)
    canvas.ellipse(32, 83, 5, 5, FROST)


def draw_ability_frost_root(canvas: Canvas) -> None:
    canvas.frame(FROST)
    for angle_index in range(6):
        angle = angle_index * math.pi / 3
        x = 64 + math.cos(angle) * 31
        y = 54 + math.sin(angle) * 31
        canvas.line([(64, 54), (x, y)], FROST_LIGHT, 4)
    canvas.ellipse(64, 54, 7, 7, HOLY_LIGHT)
    canvas.line([(64, 61), (64, 89), (45, 105)], RANGER_LIGHT, 5)
    canvas.line([(64, 83), (82, 104)], RANGER_LIGHT, 5)
    canvas.line([(64, 75), (48, 88)], RANGER, 4)
    canvas.line([(64, 72), (80, 85)], RANGER, 4)


def draw_ability_arcane_burst(canvas: Canvas) -> None:
    canvas.frame(ARCANE)
    impact_star(canvas, 64, 63, 45, 18, ARCANE_LIGHT)
    canvas.ellipse(64, 63, 25, 25, ARCANE)
    canvas.ellipse(64, 63, 14, 14, HOLY_LIGHT)
    canvas.ellipse(64, 63, 6, 6, EMBER_LIGHT)
    canvas.ellipse(29, 35, 5, 5, FROST_LIGHT)
    canvas.ellipse(101, 88, 5, 5, GOLD_LIGHT)


def draw_ability_meditate(canvas: Canvas) -> None:
    canvas.frame(ARCANE)
    canvas.ellipse(64, 36, 12, 12, HOLY_LIGHT)
    canvas.line([(64, 49), (64, 76)], ARCANE_LIGHT, 8)
    canvas.line([(64, 58), (45, 71), (35, 88)], ARCANE_LIGHT, 7)
    canvas.line([(64, 58), (83, 71), (93, 88)], ARCANE_LIGHT, 7)
    canvas.line([(64, 76), (45, 94), (33, 94)], FROST_LIGHT, 7)
    canvas.line([(64, 76), (83, 94), (95, 94)], FROST_LIGHT, 7)
    canvas.ellipse(64, 69, 44, 38, (117, 89, 211, 58))
    canvas.ellipse(64, 69, 32, 27, (18, 27, 23, 230))
    canvas.ellipse(64, 69, 5, 5, GOLD_LIGHT)


def draw_missing_ability(canvas: Canvas) -> None:
    canvas.frame(ARCANE)
    impact_star(canvas, 64, 60, 34, 16, IRON)
    canvas.line([(48, 48), (64, 36), (80, 49), (64, 63), (64, 82)], STONE_LIGHT, 6)
    canvas.ellipse(64, 96, 5, 5, ARCANE_LIGHT)


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


def draw_paper_doll_backdrop(canvas: Canvas) -> None:
    """Draw a restrained armored adventurer silhouette behind equipment slots."""

    canvas.ellipse(64, 80, 48, 73, (83, 126, 70, 24))
    canvas.ellipse(64, 80, 42, 66, (18, 27, 23, 118))
    canvas.line([(64, 8), (91, 20), (106, 48), (106, 112), (91, 140), (64, 153)], (232, 172, 61, 105), 2.0)
    canvas.line([(64, 8), (37, 20), (22, 48), (22, 112), (37, 140), (64, 153)], (232, 172, 61, 105), 2.0)
    canvas.ellipse(64, 27, 13, 15, (151, 157, 139, 205))
    canvas.polygon([(52, 24), (64, 16), (76, 24), (73, 38), (64, 43), (55, 38)], (91, 102, 99, 215))
    canvas.line([(53, 30), (75, 30)], (255, 219, 116, 150), 2.0)

    canvas.polygon([(43, 48), (56, 41), (72, 41), (85, 48), (82, 95), (64, 105), (46, 95)], (91, 102, 99, 205))
    canvas.polygon([(49, 51), (64, 45), (79, 51), (76, 88), (64, 97), (52, 88)], (151, 157, 139, 190))
    canvas.line([(45, 73), (83, 73)], (232, 172, 61, 155), 3.0)
    canvas.ellipse(64, 73, 4, 4, (243, 91, 39, 210))
    ember_rune(canvas, 64, 65, 0.52)

    canvas.polygon([(43, 49), (32, 55), (22, 95), (31, 99), (49, 69)], (83, 126, 70, 185))
    canvas.polygon([(85, 49), (96, 55), (106, 95), (97, 99), (79, 69)], (83, 126, 70, 185))
    canvas.ellipse(27, 99, 7, 6, (174, 184, 177, 190))
    canvas.ellipse(101, 99, 7, 6, (174, 184, 177, 190))

    canvas.polygon([(47, 94), (62, 98), (59, 142), (42, 146), (48, 114)], (130, 77, 42, 205))
    canvas.polygon([(66, 98), (81, 94), (80, 114), (86, 146), (69, 142)], (130, 77, 42, 205))
    canvas.line([(45, 111), (60, 111)], (174, 184, 177, 150), 2.5)
    canvas.line([(68, 111), (83, 111)], (174, 184, 177, 150), 2.5)
    canvas.polygon([(41, 143), (59, 139), (59, 151), (36, 151)], (91, 102, 99, 215))
    canvas.polygon([(69, 139), (87, 143), (92, 151), (69, 151)], (91, 102, 99, 215))

    canvas.line([(30, 119), (38, 134), (46, 140)], (255, 219, 116, 115), 2.0)
    canvas.line([(98, 119), (90, 134), (82, 140)], (255, 219, 116, 115), 2.0)


ICON_BUILDERS = {
    "T_Icon_Item_RecruitPack.png": draw_recruit_pack,
    "T_Icon_Item_MarshTonic.png": draw_marsh_tonic,
    "T_Icon_Ability_Strike.png": draw_ability_strike,
    "T_Icon_Ability_Taunt.png": draw_ability_taunt,
    "T_Icon_Ability_ShieldSlam.png": draw_ability_shield_slam,
    "T_Icon_Ability_BattleShout.png": draw_ability_battle_shout,
    "T_Icon_Ability_Smite.png": draw_ability_smite,
    "T_Icon_Ability_LesserHeal.png": draw_ability_lesser_heal,
    "T_Icon_Ability_Ward.png": draw_ability_ward,
    "T_Icon_Ability_Judgment.png": draw_ability_judgment,
    "T_Icon_Ability_QuickShot.png": draw_ability_quick_shot,
    "T_Icon_Ability_Snare.png": draw_ability_snare,
    "T_Icon_Ability_TwinCut.png": draw_ability_twin_cut,
    "T_Icon_Ability_NaturesFocus.png": draw_ability_natures_focus,
    "T_Icon_Ability_SparkBolt.png": draw_ability_spark_bolt,
    "T_Icon_Ability_FrostRoot.png": draw_ability_frost_root,
    "T_Icon_Ability_ArcaneBurst.png": draw_ability_arcane_burst,
    "T_Icon_Ability_Meditate.png": draw_ability_meditate,
    "T_Icon_Ability_Missing.png": draw_missing_ability,
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
    paper_doll = Canvas(PAPER_DOLL_WIDTH, PAPER_DOLL_HEIGHT)
    draw_paper_doll_backdrop(paper_doll)
    paper_doll.save(OUTPUT_DIR / "T_UI_PaperDoll_Backdrop.png")
    print(
        f"Generated {len(ICON_BUILDERS)} Embermere UI icons and "
        f"1 paper-doll backdrop in {OUTPUT_DIR}"
    )


if __name__ == "__main__":
    main()
