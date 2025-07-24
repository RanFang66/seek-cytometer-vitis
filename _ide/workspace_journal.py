# 2025-07-24T16:18:43.943677
import vitis

client = vitis.create_client()
client.set_workspace(path="software")

platform = client.get_component(name="seek_cytometer")
status = platform.build()

comp = client.get_component(name="cytometer_demo")
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

