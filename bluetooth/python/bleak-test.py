import asyncio
from bleak import BleakScanner

async def run():
    devices = await BleakScanner.discover()
    print("Found devices:")
    for d in devices:
        print(d)

loop = asyncio.get_event_loop()
loop.run_until_complete(run())