#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "aclk_datatype.h"
#include "aclk_queue.h"

// queue for the message received
MSG_QUEUE_T  g_st_recv_queue;

// queue for the message to send
MSG_QUEUE_T  g_st_send_queue;


/**************************************************************************************************/
/* Name   : queue_add_node                                                                         */
/* Descrp : Add message node into queue.                                                          */
/* Input  : pst_node -- the node of set msg which to be put into queue                             */
/*        : pst_msg_queue -- queue which the node to add                                            */
/* Output : none                                                                                  */
/* Return : ACLK_OK: success                                                                      */
/*          ACLK_ERROR: failure                                                                   */
/**************************************************************************************************/
INT32 queue_add_node(QUEUE_NODE_T *pst_node, MSG_QUEUE_T *pst_msg_queue)
{
    UINT32  curr_pos = 0;

    if (NULL == pst_node || NULL == pst_msg_queue)
    {
        return ACLK_ERROR;
    }
    
    pthread_mutex_lock(&(pst_msg_queue->st_mutex_lock)); 
    
	if (MAX_QUEUE_NODE_SIZE == pst_msg_queue->num)
	{
	    //ACLK_LogDump(ACLK_ERR,  __FILE__, __LINE__, "Error: Message queue is full, reach limit %d\n", pst_msg_queue->num);
	    printf("Error: Message queue is full, reach limit %d\n", pst_msg_queue->num);
	    pthread_mutex_unlock(&(pst_msg_queue->st_mutex_lock));
	    return ACLK_ERROR;
	}     

	(pst_msg_queue->num)++;

	// if set inpos is tail, then set inpos to queue head
	if (MAX_QUEUE_NODE_SIZE == pst_msg_queue->in_pos)
	{
	    pst_msg_queue->in_pos = 0;
	}

	curr_pos = pst_msg_queue->in_pos;
	memcpy(&(pst_msg_queue->st_msg_buf[curr_pos]), pst_node, sizeof(QUEUE_NODE_T));
	(pst_msg_queue->in_pos)++;
    
    pthread_mutex_unlock(&(pst_msg_queue->st_mutex_lock));
    
    return ACLK_OK;
        
}

/**************************************************************************************************/
/* Name   : queue_get_node                                                                         */
/* Descrp : Get message from queue.                                                               */
/* Input  : none                                                                                  */
/* Output : pst_node -- queue command message                                                      */
/* Return : ACLK_OK: success                                                                      */
/*          ACLK_ERROR: failure                                                                   */
/**************************************************************************************************/
INT32 queue_get_node(QUEUE_NODE_T *pst_node, MSG_QUEUE_T *pst_msg_queue)
{
    UINT32  curr_pos = 0;

    if (NULL == pst_node || NULL == pst_msg_queue) {
        return ACLK_ERROR;
    }
	
    pthread_mutex_lock(&(pst_msg_queue->st_mutex_lock));    
    
    if (0 == pst_msg_queue->num) {
        pthread_mutex_unlock(&(pst_msg_queue->st_mutex_lock));
        return ACLK_ERROR;
    }

    (pst_msg_queue->num)--;

	// if get node is tail, then set out_pos to queue head
    if (MAX_QUEUE_NODE_SIZE == pst_msg_queue->out_pos) {
        pst_msg_queue->out_pos = 0;
    }
    
    curr_pos = pst_msg_queue->out_pos;
    memcpy(pst_node, &(pst_msg_queue->st_msg_buf[curr_pos]), sizeof(QUEUE_NODE_T));
    (pst_msg_queue->out_pos)++;
    
    pthread_mutex_unlock(&(pst_msg_queue->st_mutex_lock));
    
    return ACLK_OK;
}

/**************************************************************************************************/
/* Name   : init_queue                                                                            */
/* Descrp : initial queue.                                                                        */
/* Input  : none                                                                                  */
/* Output : none                                                                                  */
/* Return : none                                                                                  */
/**************************************************************************************************/
VOID init_queue(MSG_QUEUE_T *pst_msg_queue)
{
    memset(pst_msg_queue, 0, sizeof(MSG_QUEUE_T));
    pst_msg_queue->max_size = MAX_QUEUE_NODE_SIZE;
    pthread_mutex_init(&(pst_msg_queue->st_mutex_lock), NULL);
}

