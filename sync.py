from serial import Serial
import sys
from time import time, sleep

if len(sys.argv) < 2:
    print("""
        Usage: python sync.py DEVICE
        Example: python sync.py /dev/ttyACM0
    """)
    sys.exit()
s = Serial(sys.argv[1], 9600)

while True:
    response = s.readline().strip()
    if 'Waiting for sync message' in response:
        print 'syncing'
        s.write(''.join(['T', str(int(time() + 60 * 60))]))
        s.flush()
    print repr(response)
