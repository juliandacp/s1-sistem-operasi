#!/usr/bin/env python3
import math
import os
import subprocess
import sys
from pathlib import Path

IMAGE_NAME = "bin/floppy.img"
SECTOR_SIZE = 512
MAX_SECTOR = 2880
ROOT_INDEX = 0xFF

FS_MAP_SECTOR_NUMBER = 0x100
FS_NODE_SECTOR_NUMBER = 0x101
FS_DATA_SECTOR_NUMBER = 0x103

FS_NODE_D_DIR = 0xFF
FS_MAX_NODE = 64
FS_MAX_DATA = 32
FS_MAX_SECTOR = 32
MAX_FILENAME = 14

NODE_ITEM_SIZE = 16
DATA_ITEM_SIZE = 32


def load_image(path):
    p = Path(path)
    if p.exists():
        data = p.read_bytes()
    else:
        data = b""

    expected = MAX_SECTOR * SECTOR_SIZE
    if len(data) < expected:
        data += b"\x00" * (expected - len(data))
    else:
        data = data[:expected]
    return bytearray(data)


def save_image(path, image):
    Path(path).write_bytes(bytes(image[: MAX_SECTOR * SECTOR_SIZE]))

def read_sector(image, sector):
    start = sector * SECTOR_SIZE
    return bytes(image[start:start + SECTOR_SIZE])


def write_sector(image, sector, data):
    start = sector * SECTOR_SIZE
    chunk = bytes(data[:SECTOR_SIZE]).ljust(SECTOR_SIZE, b"\x00")
    image[start:start + SECTOR_SIZE] = chunk


def fs_init(image):
    """
    Reserve:
    - sectors 0..31   : bootloader + kernel
    - sectors 256..511: filesystem metadata area
    """
    map_sec = bytearray(read_sector(image, FS_MAP_SECTOR_NUMBER))
    for i in range(32):
        map_sec[i] = 1
    for i in range(256, 512):
        map_sec[i] = 1
    write_sector(image, FS_MAP_SECTOR_NUMBER, map_sec)


def load_node_region(image):
    return bytearray(
        read_sector(image, FS_NODE_SECTOR_NUMBER) +
        read_sector(image, FS_NODE_SECTOR_NUMBER + 1)
    )


def save_node_region(image, node_region):
    write_sector(image, FS_NODE_SECTOR_NUMBER, node_region[:SECTOR_SIZE])
    write_sector(image, FS_NODE_SECTOR_NUMBER + 1, node_region[SECTOR_SIZE:])


def load_data_region(image):
    return bytearray(
        read_sector(image, FS_DATA_SECTOR_NUMBER) +
        read_sector(image, FS_DATA_SECTOR_NUMBER + 1)
    )


def save_data_region(image, data_region):
    write_sector(image, FS_DATA_SECTOR_NUMBER, data_region[:SECTOR_SIZE])
    write_sector(image, FS_DATA_SECTOR_NUMBER + 1, data_region[SECTOR_SIZE:])


def node_offset(index):
    return index * NODE_ITEM_SIZE


def data_offset(index):
    return index * DATA_ITEM_SIZE


def get_node(node_region, index):
    off = node_offset(index)
    parent = node_region[off]
    data_index = node_region[off + 1]
    raw_name = bytes(node_region[off + 2:off + 16])
    name = raw_name.split(b"\x00", 1)[0].decode("latin-1", errors="ignore")
    return {
        "parent": parent,
        "data_index": data_index,
        "name": name,
        "off": off,
    }


def set_node(node_region, index, parent, data_index, name):
    off = node_offset(index)
    node_region[off] = parent & 0xFF
    node_region[off + 1] = data_index & 0xFF
    node_region[off + 2:off + 16] = b"\x00" * 14
    raw = name.encode("latin-1", errors="ignore")[:MAX_FILENAME - 1]
    node_region[off + 2:off + 2 + len(raw)] = raw


def find_node(node_region, name, parent_index):
    for i in range(FS_MAX_NODE):
        node = get_node(node_region, i)
        if node["name"] == name and node["parent"] == (parent_index & 0xFF):
            return i, node
    return -1, None


def find_free_node(node_region):
    for i in range(FS_MAX_NODE):
        if node_region[node_offset(i) + 2] == 0:
            return i
    return -1


def find_free_data(data_region):
    for i in range(FS_MAX_DATA):
        if data_region[data_offset(i)] == 0:
            return i
    return -1


def normalize_name(path_or_name):
    base = os.path.basename(path_or_name.rstrip("/"))
    if not base:
        raise ValueError("Invalid name/path: %r" % path_or_name)
    if len(base) >= MAX_FILENAME:
        base = base[:MAX_FILENAME - 1]
    return base


def ensure_parent_ok(node_region, parent_index):
    if parent_index == ROOT_INDEX:
        return

    if parent_index < 0 or parent_index >= FS_MAX_NODE:
        raise ValueError("Invalid parent index: %d" % parent_index)

    parent = get_node(node_region, parent_index)
    if parent["name"] == "":
        raise ValueError("Parent node %d does not exist" % parent_index)
    if parent["data_index"] != FS_NODE_D_DIR:
        raise ValueError("Parent node %d is not a directory" % parent_index)


def create_directory(image, dirname, parent_index):
    fs_init(image)
    node_region = load_node_region(image)

    name = normalize_name(dirname)
    ensure_parent_ok(node_region, parent_index)

    existing, _ = find_node(node_region, name, parent_index)
    if existing != -1:
        raise ValueError("Directory already exists: %r in parent %d" % (name, parent_index))

    free_node = find_free_node(node_region)
    if free_node == -1:
        raise RuntimeError("No free node slot")

    set_node(node_region, free_node, parent_index, FS_NODE_D_DIR, name)
    save_node_region(image, node_region)
    return free_node


def insert_file(image, filename, parent_index):
    fs_init(image)
    node_region = load_node_region(image)
    data_region = load_data_region(image)
    map_region = bytearray(read_sector(image, FS_MAP_SECTOR_NUMBER))

    name = normalize_name(filename)
    ensure_parent_ok(node_region, parent_index)

    existing, _ = find_node(node_region, name, parent_index)
    if existing != -1:
        raise ValueError("File already exists: %r in parent %d" % (name, parent_index))

    content = Path(filename).read_bytes()
    sectors_needed = math.ceil(len(content) / SECTOR_SIZE) if content else 0
    if sectors_needed > FS_MAX_SECTOR:
        raise RuntimeError("File too large for one data entry: %d bytes" % len(content))

    free_node = find_free_node(node_region)
    if free_node == -1:
        raise RuntimeError("No free node slot")

    free_data = find_free_data(data_region)
    if free_data == -1:
        raise RuntimeError("No free data slot")

    free_sectors = [i for i in range(256) if map_region[i] == 0]
    if len(free_sectors) < sectors_needed:
        raise RuntimeError("Not enough free disk sectors")

    # Clear data slot first
    off = data_offset(free_data)
    data_region[off:off + FS_MAX_SECTOR] = b"\x00" * FS_MAX_SECTOR

    padded = content.ljust(sectors_needed * SECTOR_SIZE, b"\x00")
    for j, sector in enumerate(free_sectors[:sectors_needed]):
        data_region[off + j] = sector & 0xFF
        map_region[sector] = 1
        write_sector(image, sector, padded[j * SECTOR_SIZE:(j + 1) * SECTOR_SIZE])

    set_node(node_region, free_node, parent_index, free_data, name)

    write_sector(image, FS_MAP_SECTOR_NUMBER, map_region)
    save_node_region(image, node_region)
    save_data_region(image, data_region)

    return free_node


def test_1(image):
    create_directory(image, "dir1", ROOT_INDEX)
    create_directory(image, "dir2", ROOT_INDEX)
    create_directory(image, "dir1-1", 0)
    create_directory(image, "dir1-2", 0)
    create_directory(image, "dir1-1-1", 2)
    create_directory(image, "dirname", 1)
    create_directory(image, "dirname", 3)
    create_directory(image, "dir3", ROOT_INDEX)


def test_2(image):
    insert_file(image, "test/files/test_2/file-0", ROOT_INDEX)

    create_directory(image, "dir-1", ROOT_INDEX)
    for i in range(2, 63):
        create_directory(image, "dir-%d" % i, i - 1)

    insert_file(image, "test/files/test_2/file-63", ROOT_INDEX)


def test_3(image):
    insert_file(image, "test/files/test_3/1024", ROOT_INDEX)
    insert_file(image, "test/files/test_3/4096", ROOT_INDEX)

    for i in range(11):
        path = "test/files/test_3/8192_%d" % i
        print("Creating %s" % path)
        insert_file(image, path, ROOT_INDEX)

    insert_file(image, "test/files/test_3/16384", ROOT_INDEX)


def test_4(image):
    create_directory(image, "dir1", ROOT_INDEX)
    create_directory(image, "dir2", ROOT_INDEX)

    insert_file(image, "test/files/test_4/katanya", 0)

    create_directory(image, "dir3", 0)

    insert_file(image, "test/files/test_4/bikin", 3)
    insert_file(image, "test/files/test_4/fp", 3)

    create_directory(image, "dir4", 3)
    create_directory(image, "dir5", 0)

    insert_file(image, "test/files/test_4/cuma", 7)
    insert_file(image, "test/files/test_4/seminggu", 7)

    insert_file(image, "test/files/test_4/doang", ROOT_INDEX)


def test_5():
    subprocess.run(
        ["python3", "test/convert_image.py", "test/files/test_5/image.jpg", "image.jpg"],
        check=True,
    )


def main():
    if len(sys.argv) < 2:
        print("Usage: %s <test_number>" % sys.argv[0])
        return 1

    test_number = int(sys.argv[1])

    if test_number == 5:
        test_5()
        return 0

    image = load_image(IMAGE_NAME)

    if test_number == 1:
        test_1(image)
    elif test_number == 2:
        test_2(image)
    elif test_number == 3:
        test_3(image)
    elif test_number == 4:
        test_4(image)
    else:
        print("Invalid test number")
        return 1

    save_image(IMAGE_NAME, image)
    return 0

if __name__ == "__main__":
    raise SystemExit(main())