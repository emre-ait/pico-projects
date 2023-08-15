# -*- coding: utf-8 -*-
"""Utility to convert images to raw RGB565 format."""

from PIL import Image
from struct import pack
from os import path
import sys


def error(msg):
    """Display error and exit."""
    print (msg)
    sys.exit(-1)


def write_bin(f, pixel_list):
    """Save image in RGB565 format."""
    pix_list_len = len(pixel_list)
    print("pix_list length: ", pix_list_len)
    pix = pixel_list[16567]
    print("pix: ",pix)
    r = pix[0]
    print("r value: ", r)
    rshift = r >> 3
    print("r shift 3 value: ", rshift)
    rand = rshift & 0x1F
    print("r and with 0x1F (0001 1111): ", rand)
    #r = (pix[0] >> 3) & 0x1F
    #g = (pix[1] >> 2) & 0x3F
    #b = (pix[2] >> 3) & 0x1F
    #f.write(pack('>H', (r << 11) + (g << 5) + b))


if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        error('Please specify input file: ./img2rgb565.py test.png')
    in_path = args[1]
    if not path.exists(in_path):
        error('File Not Found: ' + in_path)

    filename, ext = path.splitext(in_path)
    out_path = filename + '.raw'
    img = Image.open(in_path).convert('RGB')
    pixels = list(img.getdata())
    with open(out_path, 'wb') as f:
        write_bin(f, pixels)
    print('Saved: ' + out_path)