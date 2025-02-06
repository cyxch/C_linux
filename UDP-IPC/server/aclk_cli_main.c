#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/sem.h>

#include "proc_sock_path.h"
#include "aclk_msg_process.h"
#include "aclk_queue.h"


static INT32 init_all(VOID)
{	
	// init the message queue
	init_queue(&g_st_recv_queue);
	init_queue(&g_st_send_queue);

	return ACLK_OK;
}


INT32 main(INT32 argc, INT8 *argv[])
{
	
	pthread_t msg_thread_id;
	pthread_t response_thread_id;
	INT32 rtn;
	
	// initialize the system
	init_all();

	// thread to handle message
	rtn = pthread_create(&msg_thread_id, NULL, (VOID*)srv_recv_msg_handler, NULL);
	if (ACLK_OK != rtn) {
		LOG_ERROR("Fail to create message handler thread\n");
		return ACLK_ERROR;
	}

	// thread to respond message
	rtn = pthread_create(&response_thread_id, NULL, (VOID*)srv_response_handler, NULL);
	if (ACLK_OK != rtn) {
		LOG_ERROR("Fail to create response thread\n");
		return ACLK_ERROR;
	}

    // main thread to receive message from network or serial port
    srv_entry();

    return ACLK_OK;

}
