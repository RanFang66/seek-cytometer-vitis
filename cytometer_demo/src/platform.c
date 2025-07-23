#include "xparameters.h"
#include "xil_types.h"
#include "platform.h"
#include "xscugic.h"
#include <lwip/err.h>
#include <lwip/ip_addr.h>
#include <lwip/pbuf.h>
#include <netif/xadapter.h>
#include <xil_exception.h>
#include <xstatus.h>
#include <xemacps.h>
#include "xiltimer.h"

#include "udp_server.h"

#define INTC_BASE_ADDR          XPAR_XSCUGIC_0_BASEADDR
#define INTC_DIST_BASE_ADDR     XPAR_XSCUGIC_0_DIST_BASEADDR




static XTimer timer_inst;
static XScuGic gic_inst; 
static u64_t time_ms = 0;

int send_udp_frame_flag = 0;

XStatus init_gic(XScuGic *gic, UINTPTR base_addr)
{
    XScuGic_Config *intc_config;
    XStatus status;

    intc_config = XScuGic_LookupConfig(base_addr);
    if (!intc_config) {
        xil_printf("In %s Platform Initialize Error: cannot find the configuration of GIC.\r\n", __func__);
        return XST_FAILURE;
    }

    status = XScuGic_CfgInitialize(gic, intc_config, intc_config->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        xil_printf("In %s: GIC configuration initialization failed!\r\n", __func__);
        return XST_FAILURE;
    }

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, gic);
    Xil_ExceptionEnable(); 

    xil_printf("Initialize GIC ok.\r\n");   
    return XST_SUCCESS;
}

XStatus connect_interrupt(XScuGic *gic, u32 intc_id, Xil_ExceptionHandler handler, void *callback_ref, u8 priority, u8 trigger)
{
    XStatus status;

    status = XScuGic_Connect(gic, intc_id, handler, callback_ref);
    if (status != XST_SUCCESS) {
        xil_printf("In %s: Connect interrupt failed!\r\n", __func__);
        return XST_FAILURE;
    }

    XScuGic_SetPriorityTriggerType(gic, intc_id, priority, trigger);
    XScuGic_Enable(gic, intc_id);

    return XST_SUCCESS;
}


u64_t get_platform_time()
{
    return time_ms;
}

void timer_interval_handler(void *callback_ref, u32 status_event)
{
    time_ms++;

    if (time_ms % 5 == 0) {
        send_udp_frame_flag = 1;
    }

    // if (time_ms % 10 == 0) {
    //     xil_printf("Tick for %d s!\r\n", time_ms);
    // }
}

XStatus init_timer(XTimer *timer_inst, unsigned long interval, u8 priority, XTimer_TickHandler handler, void * callback_ref)
{
    XStatus status;

    status = XilTickTimer_Init(timer_inst);

    if (status != XST_SUCCESS) {
        xil_printf("In %s: Tick timer initialization failed!\r\n");
        return XST_FAILURE;
    }

    XTimer_SetInterval(interval);
    XTimer_SetHandler(handler, callback_ref, priority);


    // timer_inst->XTimer_TickIntrHandler(timer_inst, priority);

    xil_printf("Initialize tick timer ok.\r\n");
    return XST_SUCCESS;
}



XStatus init_platform()
{
    XStatus status;
    status = init_gic(&gic_inst, INTC_BASE_ADDR);
    if (status == XST_FAILURE) {
        xil_printf("In %s-%d: Platform initialize gic failed!\n", __func__, __LINE__);
        return XST_FAILURE;
    }

    status = init_timer(&timer_inst, 1000, 0, timer_interval_handler, NULL);   
    if (status == XST_FAILURE) {
        xil_printf("In %s-%d: Platform initialize timer failed!\n", __func__, __LINE__);
        return XST_FAILURE;
    }

    status = setup_udp_server();   
    if (status == XST_FAILURE) {
        xil_printf("In %s-%d: Platform setup udp server failed!\n", __func__, __LINE__);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}







