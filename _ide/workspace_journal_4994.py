# 2025-07-21T17:12:55.270094
import vitis

client = vitis.create_client()
client.set_workspace(path="software")

advanced_options = client.create_advanced_options_dict(dt_overlay="0")

platform = client.create_platform_component(name = "seek_cytometer",hw_design = "$COMPONENT_LOCATION/../../hardware/udp_server_demo/udp_server_wrapper.xsa",os = "standalone",cpu = "psu_cortexa53_0",domain_name = "standalone_psu_cortexa53_0",generate_dtb = False,advanced_options = advanced_options,architecture = "64-bit",compiler = "gcc")

platform = client.get_component(name="seek_cytometer")
status = platform.build()

comp = client.create_app_component(name="cytometer_demo",platform = "$COMPONENT_LOCATION/../seek_cytometer/export/seek_cytometer/seek_cytometer.xpfm",domain = "standalone_psu_cortexa53_0",template = "empty_application")

status = platform.build()

status = platform.build()

comp = client.get_component(name="cytometer_demo")
comp.build()

status = platform.build()

comp.build()

vitis.dispose()

