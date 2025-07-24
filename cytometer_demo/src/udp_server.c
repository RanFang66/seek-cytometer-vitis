
#include "xil_printf.h"
#include "xparameters.h"
#include "lwipopts.h"


#include "lwip/init.h"
#include "lwip/err.h"

#include "netif/xadapter.h"
#include "udp_server.h"
#include <arch/cc.h>
#include <xstatus.h>

#include "cytometer.h"
#include "platform.h"


#define NETIF_EMAC_BASEADDR         XPAR_XEMACPS_0_BASEADDR
#define FRAME_HEADER_HMI_TO_SOC     0xA5
#define FRAME_HEADER_SOC_TO_HMI     0x5A

#define CMD_HAND_SHAKE 				0x0001
#define CMD_DETECTOR_SETTINGS 		0x0002
#define CMD_DISABLE_DETECTOR        0x0003
#define CMD_ACQUIRE_SETTINGS		0x0004
#define CMD_ACQUIRE_START			0x0005
#define CMD_ACQUIRE_STOP			0x0006
#define CMD_SORTING_SETTINGS 		0x0007
#define CMD_SORTING_START			0x0008
#define CMD_SORTING_STOP			0x0009

#define CMD_PULSE_DATA				0x0010
#define CMD_WAVEFORM_DATA			0x0020




/* Is connected with HMI */
int is_connected = 0;
int comm_lost_timer = 0;

static unsigned char mac_ethernet_address[] = {0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
static struct netif netif_inst;
static struct udp_pcb *server_pcb;
static u16_t  sequence_val = 0;
static u16_t  sequence_last = 0;
static u16_t  sequence_recv = 0;
static u16_t  sequence_recv_last = 0;
static u8_t   hmi_state = 0;

ip_addr_t       target_ip;
u16_t           target_port;
static u16_t    server_port = 5001;

extern int send_udp_frame_flag;

static void print_ip(const char *msg, const ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

static void print_ip_settings(const struct netif *netif)
{
    print_ip("Board IP:       ", &(netif->ip_addr));
	print_ip("Netmask :       ", &(netif->netmask));
	print_ip("Gateway :       ", &(netif->gw));
}

XStatus init_udp_server(struct netif *netif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway, UINTPTR mac_baseaddr)
{
    if (!netif) {
        xil_printf("In %s: Error the netif instance is NULL!\r\n", __func__);
        return XST_FAILURE;
    }

    lwip_init();

    if (!xemac_add(netif, ipaddr, netmask, gateway, mac_ethernet_address, mac_baseaddr)) {
        xil_printf("In %s: Add N/W interface error!\r\n", __func__);
        return XST_FAILURE;
    }

    netif_set_default(netif);

    netif_set_up(netif);
    if (netif_is_up(netif)) {
        xil_printf("Network interface is up!\n");
    } else {
        xil_printf("In %s: Cannot up network interface!\r\n", __func__);
        return XST_FAILURE;
    }

    print_ip_settings(netif);

    xil_printf("\nInitialize network interface ok!\n");

    return XST_SUCCESS;
}


XStatus start_udp_server(struct udp_pcb *server_pcb, const ip_addr_t *ip_addr, u16_t port, udp_recv_fn udp_recv_handler, void *recv_arg)
{
    err_t  err;

    err = udp_bind(server_pcb, ip_addr, port);
    if (err != ERR_OK) {
        xil_printf("In %s: Error bind udp server to port %d, err_no = %d.\r\n", __func__, port, err);
        udp_remove(server_pcb);
        return XST_FAILURE;
    }
    xil_printf("bind UDP server to port %d ok!\n", port);

    udp_recv(server_pcb, udp_recv_handler, recv_arg);

    return XST_SUCCESS;
}

u32_t big_endian_to_little_endian(const u8_t *bytes)
{
	return ((u32_t)bytes[0] << 24) |
			((u32_t)bytes[1] << 16) |
			((u32_t)bytes[2] << 8) |
			((u32_t)bytes[3]);
}


static u16_t  checksum(const u8_t *pdata, int len)
{
	u16_t sum = 0;
	for (int i = 0; i < len; i++) {
		sum += (u8_t)pdata[i];
	}
	return sum;
}

static XStatus parse_frame(const u8_t *pdata, int len)
{
    if (len < 9) {
		xil_printf("Udp Server Error: Frame size is less than 9!\n");
		return XST_FAILURE;
	}

	u8_t frame_head = (u8_t)pdata[0];
	if (frame_head != FRAME_HEADER_HMI_TO_SOC) {
		xil_printf("Udp Server Error: Frame header error!\n");
		return XST_FAILURE;
	}

	u16_t chk_sum = checksum(pdata, len - 2);
	if (chk_sum !=  (((u16_t)pdata[len-2] << 8) | (u16_t)pdata[len-1])) {
		xil_printf("Udp Server Error: Frame check sum error!\n");
		return XST_FAILURE;
	}

    sequence_recv = ((u16_t)pdata[1] << 8) | (u16_t)pdata[2];
	u16_t cmd_type = ((u16_t)pdata[3] << 8) | (u16_t)pdata[4];
	u16_t data_len = ((u16_t)pdata[5] << 8) | (u16_t)pdata[6];

    if (data_len != len - 9) {
        xil_printf("Udp Server Error: Frame size is not match with data size!\n");
		return XST_FAILURE;
    }

    const u8_t *data_start = pdata+7;
	switch (cmd_type) {
	case CMD_HAND_SHAKE:
	{	        
        u8_t state = (u8_t)data_start[0];
        if (state != hmi_state) {
            xil_printf("In %d s, hmi state changed from %d to %d.\r\n", get_platform_time(), hmi_state, state);
            hmi_state = state;
        }
		break;
    }
	case CMD_DETECTOR_SETTINGS:
    {
		for (int i = 0; i < data_len / 9; i++) {
			u8_t id = (u8_t)data_start[9*i];
			u8_t enable_height = (u8_t)data_start[9*i + 1];
			u8_t enable_width = (u8_t)data_start[9*i + 2];
			u8_t enable_area = (u8_t)data_start[9*i + 3];
			u8_t enable_thresh = (u8_t)data_start[9*i + 4];
			int threshold_val = big_endian_to_little_endian(data_start + 9*i+5);

			if (id <= AD_CHANNEL_NUM && id > 0) {
				update_detector_settings(id, enable_height, enable_width, enable_area, enable_thresh, threshold_val);
			}
		}
        xil_printf("In %d s, Detector settings changed.\r\n", get_platform_time());
		break;
    }
    case CMD_DISABLE_DETECTOR:
    {
        u8_t id = (u8_t)data_start[0];
        reset_detector(id);
        break;
    }
    
	case CMD_ACQUIRE_SETTINGS:
	break;
	case CMD_ACQUIRE_START: 
    {
        cytometer_start_analyze();
        xil_printf("In %d s, Start analyzing.\r\n", get_platform_time());
        break;
    }

	case CMD_ACQUIRE_STOP:
    {
        cytometer_stop_analyze();
        xil_printf("In %d s, Stop analyzing.\r\n", get_platform_time());

        break;
    }

	case CMD_SORTING_SETTINGS:
	break;
	case CMD_SORTING_START:
    {
        cytometer_start_sort();
        xil_printf("In %d s, Start sorting.\r\n", get_platform_time());

        break;
    }

	case CMD_SORTING_STOP:
    {
        cytometer_stop_sort();
        xil_printf("In %d s, Stop sorting.\r\n", get_platform_time());

    	break;
    }

	case CMD_PULSE_DATA:
	break;
    
	case CMD_WAVEFORM_DATA:
    {
		u8_t waveform_en = (u8_t)(data_start[0]);
        u8_t waveform_channels = (u8_t)(data_start[1]);
        update_waveform_setting(waveform_en, waveform_channels);
        break;
    }
		
	default:
		xil_printf("Udp Server Error: Undefined command type!\n");
		return XST_FAILURE;
		break;
	}

	return XST_SUCCESS;
}


static void udp_recv_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

    if (!p) {
        xil_printf("UDP server received NULL pbuf!\r\n");
        return;
    }

    if (p->len < 9) {
        xil_printf("UDP server received frame, but frame length is %d less then 9!\r\n", p->len);
        pbuf_free(p);
        return;
    }

    u8_t *pdata;
    pdata = (u8_t*)(p->payload);
    parse_frame(pdata, p->len);

    pbuf_free(p);

}



XStatus setup_udp_server()
{
    XStatus status;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gateway;
    IP_ADDR4(&ipaddr, 192, 168, 8, 10);
    IP_ADDR4(&netmask, 255, 255, 255, 0);
    IP_ADDR4(&gateway, 192, 168, 8, 1);

    IP4_ADDR(&target_ip, 192, 168, 8, 35);
    target_port = 8080;

    status = init_udp_server(&netif_inst, &ipaddr, &netmask, &gateway, NETIF_EMAC_BASEADDR);
    if (status == XST_FAILURE) {
        xil_printf("In %s-%d: Platform initialize udp server failed!\n", __func__, __LINE__);
        return XST_FAILURE;
    }

    server_pcb = udp_new();
    if (!server_pcb) {
        xil_printf("In %s-%d: Error creating UDP PCB.\r\n", __func__, __LINE__);
        return XST_FAILURE;
    }
    status = start_udp_server(server_pcb, IP_ADDR_ANY, server_port, udp_recv_handler, server_pcb); 
    if (status == XST_FAILURE) {
        xil_printf("In %s-%d: Platform startup udp server failed!\n", __func__, __LINE__);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}


XStatus send_udp_frame(u16_t cmd_type, u16_t data_len, const u8_t *data)
{
    struct pbuf *p;
    err_t ret;

    p = pbuf_alloc(PBUF_TRANSPORT, data_len + 9, PBUF_POOL);
    if (!p) {
        xil_printf("In %s-%d: Allocate pbuf failed!\r\n", __func__, __LINE__);
        return XST_FAILURE;
    }

    ((u8_t *) p->payload)[0] = FRAME_HEADER_SOC_TO_HMI;
    ((u8_t *) p->payload)[1] = ((u8_t)(sequence_val >> 8));
	((u8_t *) p->payload)[2] = ((u8_t)(sequence_val));
	((u8_t *) p->payload)[3] = ((u8_t)(cmd_type >> 8));
	((u8_t *) p->payload)[4] = ((u8_t)(cmd_type));
	((u8_t *) p->payload)[5] = ((u8_t)(data_len >> 8));
	((u8_t *) p->payload)[6] = ((u8_t)(data_len));
    if (data_len > 0 && data) {
	    memcpy(((u8*)p->payload + 7), data, data_len);
    }
    u16_t chksum = checksum((u8_t*)p->payload, data_len + 7);
    ((u8_t *) p->payload)[data_len+7] = ((u8_t)(chksum >> 8));
    ((u8_t *) p->payload)[data_len+8] = ((u8_t)(chksum));
    p->len = p->tot_len = 9+data_len;
    ret = udp_sendto(server_pcb, p, &target_ip, target_port);
    pbuf_free(p);
    sequence_val++;
    if (ret != ERR_OK) {
        xil_printf("Send Udp frame error! err_no = %d\r\n", ret);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

XStatus send_handshake()
{
    u8_t send_data[1];
    send_data[0] = (u8_t)(get_current_state());
    return send_udp_frame(CMD_HAND_SHAKE, 1, send_data);
}


void lwip_loop_func()
{
    int ret = 0;
    xemacif_input(&netif_inst);
    
    if (send_udp_frame_flag == 1) {
        send_udp_frame_flag = 0;
        if (sequence_last == sequence_val) {
            ret = send_handshake();
        }
        sequence_last = sequence_val;

        if (is_connected) {
            if (sequence_recv_last == sequence_recv) {
                comm_lost_timer += 2;
                if (comm_lost_timer > 10) {
                    is_connected = 0;
                    comm_lost_timer = 0;
                    xil_printf("At %d s, Communication with HMI is lost!\r\n", get_platform_time());
                }
            } else {
                comm_lost_timer = 0;
            }
        } else {
            comm_lost_timer = 0;
            if (sequence_recv_last != sequence_recv) {
                is_connected = 1;
                xil_printf("At %d s, Communication with HMI connected!\r\n", get_platform_time());
            }
        }
        sequence_recv_last = sequence_recv;
    }
}

