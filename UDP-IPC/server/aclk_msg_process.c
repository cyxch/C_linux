#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/un.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdio.h>

#include "aclk_msg_process.h"
#include "aclk_queue.h"
#include "aclk_node_process.h"
#include "proc_sock_path.h"



extern const CMD_INTERFACE_T  g_cmd_tbl[];

static INT32 g_sock_fd = -1;


static INT32 get_fd(VOID)
{
	return g_sock_fd;
}

static VOID set_fd(INT32 sock_fd)
{
	g_sock_fd = sock_fd;
}


/***FUNC+******************************************************************************************/
/* Name   : ParseMsg                                                                              */
/* Descrp : parse cmd msg and execute operation for cmd msg                                       */
/* Input  : NULL                                                                                  */
/* Output : NULL                                                                                  */
/* Return : NULL                                                                                  */
/***FUNC-******************************************************************************************/
static INT32 parse_msg(UINT8 *command, UINT32 msg_len, struct sockaddr_in * pst_sa_in, struct sockaddr_un * pst_sa_un)
{
    QUEUE_NODE_T   st_node;
    INT32 rtn;

	if (NULL == command) {
		return ACLK_ERROR;
	}
	
	memset(&st_node, 0, sizeof(st_node));

	// if recvd message is over 1024, truncated to 1024
	if (msg_len >= BUF_LEN_1024 ) {
		msg_len = BUF_LEN_1024;  // leave one place for '\0'
	}
	st_node.msg_len = msg_len;
	
	if (pst_sa_in != NULL) {
		//memcpy(&(st_node.st_destination), pst_sa_in, sizeof(st_node.st_destination));
	}

	if (pst_sa_un != NULL) {	
		memcpy(st_node.sock_path, pst_sa_un->sun_path, strlen(pst_sa_un->sun_path));
	}
	
	
	// get the message body info
	snprintf(st_node.msg_buf, BUF_LEN_1024, "%s", command);
	        
    rtn = queue_add_node(&st_node, &g_st_recv_queue);
	if (ACLK_ERROR == rtn) {
		
		return ACLK_ERROR;
	}

    return ACLK_OK;
}




VOID srv_response_handler(VOID)
{
	INT32 fd;
    QUEUE_NODE_T   st_node;
	INT32 rtn;

	while(1) {
		fd = get_fd();

		memset(&st_node, 0, sizeof(st_node));
		// get the node from send queue
		rtn = queue_get_node(&st_node, &g_st_send_queue);	
		if(ACLK_ERROR == rtn) {
			continue;  // empty queue, do nothing
		}
		
		// send back msg to source socket
		if (st_node.sock_path[0] != 0) {			
			rtn = proc_send_msg(fd, st_node.msg_buf, st_node.msg_len, st_node.sock_path);
			LOG_INFO("ccm send response msg:%s %s\n", st_node.msg_buf, st_node.sock_path);
		}
	
	}
	
}

/***FUNC+******************************************************************************************/
/* Name   : srv_recv_msg_handler                                                                    */
/* Descrp :                                                                                       */
/* Input  : None                                                                                  */
/* Output : None                                                                                  */
/* Return : None                                                                                  */
/***FUNC-******************************************************************************************/
VOID srv_recv_msg_handler(VOID)
{
    QUEUE_NODE_T  st_node;
    INT32  rtn;
        
    while(ACLK_TRUE) {
        rtn = queue_get_node(&st_node, &g_st_recv_queue);   
        if (ACLK_OK == rtn) {
			rtn = node_format(&st_node);
			if (ACLK_ERROR == rtn)
			{
				continue;
			}
			node_depart(&st_node, g_cmd_tbl);
        } else {
            
        }
		
    }

}



/***FUNC+******************************************************************************************/
/* Name   : srv_entry                                                                             */
/* Descrp : create udp socket, parse and execute user command.                                    */
/* Input  : NULL                                                                                  */
/* Output : NULL                                                                                  */
/* Return : Init UDP server success else ACLK_ERROR                                               */
/***FUNC-******************************************************************************************/
INT32 srv_entry(VOID)
{ 
    INT32 internal_sock = -1;
    INT32 rtn = ACLK_NULL;
    INT32 max_fd = ACLK_NULL;
    fd_set      st_set;              
    struct timeval st_timeout; 
	
	struct sockaddr_in st_sa_in_from_net;
	struct sockaddr_un st_sa_un_from_uart;
	INT32 addrLen = sizeof(st_sa_un_from_uart);

	INT8 data_buf[BUF_LEN_1024];
	
	
	internal_sock = proc_create_unix_sock(SRV_SOCK_PATH);
	if (internal_sock < 0) {
		LOG_ERROR("failed create sock!\n");
		return ACLK_ERROR;
	}
	
	set_fd(internal_sock);

			
    // main loop that listens  message and invokes process function
    while (ACLK_TRUE) {       
		// clear fdset
		FD_ZERO(&st_set);
		FD_SET(internal_sock, &st_set);
		
        // if NULL, fully blocked, if 0, non-blocked, if time, wait time out
        // 1 second for time out
        st_timeout.tv_sec = 1;
        st_timeout.tv_usec = 0;

		max_fd = internal_sock;

		rtn = select(max_fd + 1, &st_set, NULL, NULL, &st_timeout);
		if (rtn <= 0) {
			continue;  // socket no data
		}

		memset(data_buf, 0, sizeof(data_buf));
		memset(&st_sa_in_from_net, 0, sizeof(st_sa_in_from_net));
		memset(&st_sa_un_from_uart, 0, sizeof(st_sa_un_from_uart));

		if (FD_ISSET(internal_sock, &st_set)) { //serial socket ready, got data on socket	
			rtn = recvfrom(internal_sock, data_buf, sizeof(data_buf)-1, MSG_WAITALL, (struct sockaddr *)&st_sa_un_from_uart, (socklen_t*)&addrLen);
			printf("ccm: recv form server msg %s and socketpath: %s\n", data_buf, st_sa_un_from_uart.sun_path);
		}
		
		parse_msg((UINT8 *)data_buf, rtn, &st_sa_in_from_net, &st_sa_un_from_uart);			     
    }
 
    return ACLK_OK;    
}



