#ifndef __ACLK_QUEUE_H__
#define __ACLK_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>

#include "aclk_datatype.h"

// Max node number of queue
#define   MAX_QUEUE_NODE_SIZE       (300)

#define   CMD_EXE_FLAG_INIT         (0)
#define   CMD_EXE_FLAG_EXECUTED     (1)
#define   CMD_EXE_FLAG_WAITING      (2)

// queue node
typedef struct QUEUE_CMD_t
{
	UINT32 msg_oid;										  /*message id */
	BOOL has_msg;                                     /*flag indicating whether has message or empty node */
	struct sockaddr_in  st_reply_source;                    /*source Ip where the data reply from    */
	struct sockaddr_in  st_destination;                    /*destination where the data will be sent to  */
	INT8 sock_path[BUF_LEN_32];                          /* unix domain socket destination */
	UINT32 msg_len;                                         /*length of message.                          */
	INT8 msg_buf[BUF_LEN_1024];                          /*buffer holds command message.               */
} QUEUE_NODE_T;

// queue structure
typedef struct MSG_QUEUE_t
{
	UINT32 in_pos;                                          /*the position at which to be put message     */
	UINT32 out_pos;                                         /*the position at which to be pop set message */
	UINT32 num;                                            /*the index of unprocessed member             */
	UINT32 max_size;                                        /*the limit of queue member's number          */
	pthread_mutex_t st_mutex_lock;                          /* Mutex lock.                                */
	QUEUE_NODE_T  st_msg_buf[MAX_QUEUE_NODE_SIZE];         /*member's content                            */
}MSG_QUEUE_T;


extern MSG_QUEUE_T  g_st_recv_queue;
extern MSG_QUEUE_T  g_st_send_queue;


INT32 queue_add_node(QUEUE_NODE_T *pst_node, MSG_QUEUE_T *pst_msg_queue);
INT32 queue_get_node(QUEUE_NODE_T *pst_node, MSG_QUEUE_T *pst_msg_queue);
VOID init_queue(MSG_QUEUE_T *pst_msg_queue);

#ifdef __cplusplus
}
#endif

#endif

