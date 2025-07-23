# 2025-07-23T09:27:21.594101
import vitis

client = vitis.create_client()
client.set_workspace(path="software")

comp = client.get_component(name="cytometer_demo")
status = comp.clean()

platform = client.get_component(name="seek_cytometer")
status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

domain = platform.get_domain(name="standalone_psu_cortexa53_0")

status = domain.set_config(option = "lib", param = "lwip220_udp_debug", value = "true", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_pbuf_pool_bufsize", value = "2000", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_pbuf_pool_size", value = "2048", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_dhcp", value = "true", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_lwip_dhcp_does_acd_check", value = "true", lib_name="lwip220")

status = domain.regenerate()

status = platform.build()

status = comp.clean()

status = platform.build()

comp.build()

