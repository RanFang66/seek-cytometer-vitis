# 2025-07-23T15:45:55.255522
import vitis

client = vitis.create_client()
client.set_workspace(path="software")

comp = client.get_component(name="cytometer_demo")
status = comp.clean()

platform = client.get_component(name="seek_cytometer")
status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

vitis.dispose()

