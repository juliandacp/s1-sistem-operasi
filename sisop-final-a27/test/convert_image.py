#!/usr/bin/env python3
import sys
import os
import struct
import math
from PIL import Image

# Note: Some of the code was written with the help of LLM.

SECTOR_SIZE = 512

FS_MAX_SECTOR = 32
FS_MAX_FILE = FS_MAX_SECTOR * SECTOR_SIZE

HEADER_SIZE = 4
VGA_PALETTE_SIZE = 768
IMAGE_DATA_OFFSET = HEADER_SIZE + VGA_PALETTE_SIZE
MAX_PIXELS = FS_MAX_FILE - IMAGE_DATA_OFFSET

VGA_WIDTH = 320
VGA_HEIGHT = 200

FS_MAP_SECTOR = 0x100
FS_NODE_SECTOR = 0x101
FS_DATA_SECTOR = 0x103

FS_MAX_NODE = 64
FS_MAX_DATA = 32
FS_NODE_SIZE = 16
FS_NAME_SIZE = 14
FS_NODE_P_ROOT = 0xFF

RESERVED_SECTORS = 32
FLOPPY_PATH = "bin/floppy.img"


def resize_image(image):
    """
    Resize image to fit VGA 13h screen first, then resize again if the
    converted image would still be too large for the filesystem.
    """
    # Fit image inside 320x200 while preserving aspect ratio.
    image.thumbnail((VGA_WIDTH, VGA_HEIGHT), Image.LANCZOS)

    width, height = image.size
    pixel_count = width * height

    # Filesystem max file size is 16 KB.
    # Since header + palette already uses 772 bytes,
    # max pixel data is FS_MAX_FILE - 772.
    if pixel_count > MAX_PIXELS:
        scale = math.sqrt(MAX_PIXELS / pixel_count)

        width = max(1, int(width * scale))
        height = max(1, int(height * scale))

        image = image.resize((width, height), Image.LANCZOS)

    return image


def convert_image(src_path):
    image = Image.open(src_path).convert("RGB")
    original_width, original_height = image.size

    image = resize_image(image)

    width, height = image.size
    pixel_count = width * height

    if pixel_count > MAX_PIXELS:
        raise ValueError(
            f"Image is still too large: {width}x{height}={pixel_count} pixels"
        )

    print(
        f"  Original: {original_width}x{original_height}"
        f"  -> Resized: {width}x{height}"
        f"  ({pixel_count} pixels)"
    )

    # Quantize to 255 colors because palette index 0 is reserved for black.
    quantized = image.quantize(
        colors=255,
        dither=Image.Dither.FLOYDSTEINBERG,
    )

    # PIL palette is RGB 8-bit, usually 768 bytes.
    # Pad it to be safe, then take only 255 entries because index 0 is black.
    raw_palette = quantized.getpalette() or []
    raw_palette = (raw_palette + [0] * VGA_PALETTE_SIZE)[:VGA_PALETTE_SIZE]

    # Force palette index 0 to black, then append 255 quantized colors.
    palette_8bit = [0, 0, 0] + raw_palette[:765]

    # Convert RGB 8-bit 0..255 to VGA DAC 6-bit 0..63.
    palette_6bit = bytes(
        (value * 63 + 127) // 255
        for value in palette_8bit
    )

    # Shift pixel indices 0..254 to 1..255.
    # Index 0 is reserved for black background.
    pixels = bytes(
        min(pixel + 1, 255)
        for pixel in quantized.tobytes()
    )

    header = struct.pack("<HH", width, height)
    raw_data = header + palette_6bit + pixels

    sector_count = math.ceil(len(raw_data) / SECTOR_SIZE)

    print(f"  File size: {len(raw_data)} bytes ({sector_count} sectors)")

    if len(raw_data) > FS_MAX_FILE:
        raise ValueError(
            f"File too large: {len(raw_data)} > {FS_MAX_FILE}"
        )

    return raw_data


def inject_file(floppy_path, raw_data, dest_name):
    with open(floppy_path, "rb") as f:
        disk = bytearray(f.read())

    map_start = FS_MAP_SECTOR * SECTOR_SIZE
    node_start = FS_NODE_SECTOR * SECTOR_SIZE
    data_start = FS_DATA_SECTOR * SECTOR_SIZE

    map_buf = bytearray(disk[map_start:map_start + SECTOR_SIZE])
    node_buf = bytearray(disk[node_start:node_start + 2 * SECTOR_SIZE])
    data_buf = bytearray(disk[data_start:data_start + 2 * SECTOR_SIZE])

    name_bytes = dest_name.encode("ascii")[:FS_NAME_SIZE]

    # Check duplicate file in root directory.
    for node_index in range(FS_MAX_NODE):
        offset = node_index * FS_NODE_SIZE

        parent_index = node_buf[offset]
        node_name = node_buf[offset + 2:offset + 16].rstrip(b"\x00")

        if parent_index == FS_NODE_P_ROOT and node_name == name_bytes and node_name:
            print(
                f"Error: '{dest_name}' already exists. "
                f"Delete it in the OS first: rm {dest_name}"
            )
            return False

    # Find free node.
    free_node = -1
    for node_index in range(FS_MAX_NODE):
        offset = node_index * FS_NODE_SIZE

        if node_buf[offset + 2] == 0:
            free_node = node_index
            break

    if free_node == -1:
        print("Error: no free node slots")
        return False

    # Find free data slot.
    free_data = -1
    for data_index in range(FS_MAX_DATA):
        offset = data_index * FS_MAX_SECTOR

        if data_buf[offset] == 0:
            free_data = data_index
            break

    if free_data == -1:
        print("Error: no free data slots")
        return False

    sector_count = math.ceil(len(raw_data) / SECTOR_SIZE)

    if sector_count > FS_MAX_SECTOR:
        print(f"Error: {sector_count} sectors needed but max is {FS_MAX_SECTOR}")
        return False

    # Find free disk sectors.
    free_sectors = []

    for sector in range(RESERVED_SECTORS, 256):
        if map_buf[sector] == 0:
            free_sectors.append(sector)

        if len(free_sectors) == sector_count:
            break

    if len(free_sectors) < sector_count:
        print(f"Error: not enough free disk sectors, need {sector_count}")
        return False

    padded_data = raw_data.ljust(sector_count * SECTOR_SIZE, b"\x00")

    # Clear data slot before filling it.
    data_slot_offset = free_data * FS_MAX_SECTOR
    for i in range(FS_MAX_SECTOR):
        data_buf[data_slot_offset + i] = 0

    # Write file data.
    for index, sector in enumerate(free_sectors):
        src_start = index * SECTOR_SIZE
        src_end = src_start + SECTOR_SIZE

        dst_start = sector * SECTOR_SIZE
        dst_end = dst_start + SECTOR_SIZE

        disk[dst_start:dst_end] = padded_data[src_start:src_end]

        map_buf[sector] = 1
        data_buf[data_slot_offset + index] = sector

    # Write node entry.
    node_offset = free_node * FS_NODE_SIZE

    node_buf[node_offset] = FS_NODE_P_ROOT
    node_buf[node_offset + 1] = free_data
    node_buf[node_offset + 2:node_offset + 16] = name_bytes.ljust(
        FS_NAME_SIZE,
        b"\x00",
    )

    # Flush metadata.
    disk[map_start:map_start + SECTOR_SIZE] = map_buf
    disk[node_start:node_start + 2 * SECTOR_SIZE] = node_buf
    disk[data_start:data_start + 2 * SECTOR_SIZE] = data_buf

    with open(floppy_path, "wb") as f:
        f.write(disk)

    print(
        f"  Injected '{dest_name}' at node {free_node} "
        f"(data slot {free_data}, sectors {free_sectors})"
    )

    return True


def main():
    src_path = (
        sys.argv[1]
        if len(sys.argv) > 1
        else os.path.join(os.getcwd(), "test", "files", "test_5", "image.jpg")
    )

    # If destination name is not provided, use image filename, max 14 chars.
    dest_name = (
        sys.argv[2]
        if len(sys.argv) > 2
        else os.path.basename(src_path)[:FS_NAME_SIZE]
    )

    if not os.path.exists(src_path):
        print(f"Error: '{src_path}' not found")
        sys.exit(1)

    if not os.path.exists(FLOPPY_PATH):
        print(f"Error: '{FLOPPY_PATH}' not found — run 'make build' first")
        sys.exit(1)

    try:
        print(f"Converting '{src_path}'...")
        raw_data = convert_image(src_path)

        print(f"Injecting into '{FLOPPY_PATH}' as '{dest_name}'...")
        if not inject_file(FLOPPY_PATH, raw_data, dest_name):
            sys.exit(1)

    except Exception as error:
        print(f"Error: {error}")
        sys.exit(1)


if __name__ == "__main__":
    main()