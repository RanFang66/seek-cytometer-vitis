#include "xparameters.h"

#include "xil_printf.h"


#include "platform.h"
#include <netif/xadapter.h>
#include "udp_server.h"

int main()
{
    init_platform();

    xil_printf("-----------Start Seek Cytometer----------");

    while (1)
    {
        lwip_loop_func();
    }

    return 0;
}