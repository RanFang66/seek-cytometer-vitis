# 2025-07-22T09:17:37.608595
import vitis

client = vitis.create_client()
client.set_workspace(path="software")

platform = client.get_component(name="seek_cytometer")
domain = platform.get_domain(name="standalone_psu_cortexa53_0")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "psu_ttc_0", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_sleep_timer", value = "psu_ttc_3", lib_name="xiltimer")

status = platform.build()

status = domain.regenerate()

status = domain.set_config(option = "lib", param = "XILTIMER_sleep_timer", value = "Default", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_sleep_timer", value = "psu_ttc_3", lib_name="xiltimer")

status = platform.build()

comp = client.get_component(name="cytometer_demo")
status = comp.clean()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = domain.regenerate()

status = platform.build()

status = platform.build()

status = platform.build()

comp.build()

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

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = domain.regenerate()

status = platform.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../hardware/udp_server_demo/udp_server_wrapper.xsa")

status = domain.regenerate()

status = platform.build()

status = platform.build()

status = platform.build()

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "None", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_sleep_timer", value = "Default", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "false", lib_name="xiltimer")

status = platform.build()

status = domain.remove_lib(lib_name="xiltimer")

status = domain.set_lib(lib_name="xiltimer", path="/opt/Xilinx/2025.1/Vitis/data/embeddedsw/lib/sw_services/xiltimer_v2_2")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "psu_ttc_0", lib_name="xiltimer")

status = platform.build()

status = domain.regenerate()

status = platform.build()

client.delete_component(name="seek_cytometer")

advanced_options = client.create_advanced_options_dict(dt_overlay="0")

platform = client.create_platform_component(name = "seek_cytometer",hw_design = "$COMPONENT_LOCATION/../../hardware/udp_server_demo/udp_server_wrapper.xsa",os = "standalone",cpu = "psu_cortexa53_0",domain_name = "standalone_psu_cortexa53_0",generate_dtb = False,advanced_options = advanced_options,architecture = "64-bit",compiler = "gcc")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "psu_ttc_3", lib_name="xiltimer")

status = platform.build()

status = comp.clean()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = domain.set_lib(lib_name="lwip220", path="/opt/Xilinx/2025.1/Vitis/data/embeddedsw/ThirdParty/sw_services/lwip220_v1_2")

status = platform.build()

status = domain.set_config(option = "lib", param = "XILTIMER_sleep_timer", value = "psu_ttc_3", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "psu_ttc_0", lib_name="xiltimer")

status = domain.regenerate()

status = platform.build()

status = platform.build()

comp.build()

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

status = comp.clean()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

vitis.dispose()

