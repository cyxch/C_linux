#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <pthread.h>
#include <malloc.h>

#include "aclk_datatype.h"
#include "aclk_node_process.h"
#include "proc_sock_path.h"


extern const CMD_INTERFACE_T  g_cmd_tbl[];



/********* 1.customer board commads begin******************************************/
static INT32 test(UINT8 oid_type, UINT8 ucOAindex, UINT32 argc, INT8 *cmd_body[], INT8 *response, INT32 buf_len)
{
	snprintf(response, buf_len, ">%s:success!", cmd_body[0]);

	return ACLK_OK;
}





/********* 2.debug commads end******************************************/
const CMD_INTERFACE_T  g_cmd_tbl[] =
{  
	{"test",        PARA_NUM_1, PARA_NUM_1, 	CMD_BOARD|OID_SHOW, 					test, 			"test example"},
    {NULL,			0,			0,				0, 										NULL, 			NULL}
};

