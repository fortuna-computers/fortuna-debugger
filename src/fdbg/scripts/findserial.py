#!/usr/bin/env python3

# This script will attempt to find either the MEGA2560 debugger or the Pico I/O board 
# and return the serial port.

import re
import sys
import platform
import subprocess

if len(sys.argv) != 3:
    print("Usage: " + sys.argv[0] + " VENDOR_ID PRODUCT_ID")
    sys.exit(1)

p_vendor_id = sys.argv[1]
p_product_id = sys.argv[2]

if platform.system() == "Darwin":
    result = subprocess.run("""
       ioreg -r -c IOUSBHostDevice -x -l | perl -ne 'BEGIN {} /"USB Serial Number" = "(.+)"/ && ($sn=$1); /"idProduct" = (.+)/ && ($ip=$1); /"idVendor" = (.+)/ && ($iv=$1); /"IOCalloutDevice" = "(.+)"/ && print "$sn,$ip,$iv,$1\n"'
    """, stdout=subprocess.PIPE, shell=True)
    p_vendor_id = hex(int(p_vendor_id, 16))
    p_product_id = hex(int(p_product_id, 16))
    for line in result.stdout.decode("utf-8").splitlines():
        usb_id, product_id, vendor_id, serial_port = line.split(",")
        found = False
        if product_id == p_product_id and vendor_id == p_vendor_id:
            print(serial_port)
            sys.exit(0)
    print("Device not found")
    sys.exit(1)

elif platform.system() == "Linux":
    result = subprocess.run("find /sys/bus/usb/devices/usb*/ -name dev", stdout=subprocess.PIPE, shell=True)
    for sysdevpath in result.stdout.decode("utf-8").rstrip().split("\n"):
        result2 = subprocess.run("udevadm info -q name -p " + sysdevpath.removesuffix("/dev"), stdout=subprocess.PIPE, shell=True)
        devname = result2.stdout.decode("utf-8").rstrip()
        if not devname.startswith("bus/"):
            result3 = subprocess.run("udevadm info -q property --export -p " + sysdevpath.removesuffix("/dev"), stdout=subprocess.PIPE, shell=True)
            vrs = result3.stdout.decode("utf-8").rstrip()
            if "ID_VENDOR_ID" in vrs and "ID_MODEL_ID" in vrs:
                vendor_id = re.search("""ID_VENDOR_ID='(....)""", vrs).group(1)
                product_id = re.search("""ID_MODEL_ID='(....)""", vrs).group(1)
                if product_id == p_product_id and vendor_id == p_vendor_id:
                    print("/dev/" + devname)
                    sys.exit(0)

    print("Device not found")
    sys.exit(1)

else:
    print("Autodetect not support for this operating system.")
    sys.exit(1)