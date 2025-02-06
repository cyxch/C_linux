#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


#include "aclk_queue.h"
#include "aclk_node_process.h"
#include "proc_sock_path.h"


INT32 add_resp_2_send_queue(INT8 *response, INT32 length, QUEUE_NODE_T *pst_recv_node)
{
	QUEUE_NODE_T   st_send_node ;
	INT32 rtn; 

	if (NULL == response) {
		return ACLK_ERROR;
	}
	
	memset(&st_send_node, 0, sizeof(st_send_node) );
	st_send_node.msg_oid = pst_recv_node->msg_oid;
	
	// if recvd message is over 1024, truncated to 1024
	if (length >= BUF_LEN_1024) {
		length = BUF_LEN_1024;  // leave one place for '\0'
	}
	st_send_node.msg_len = length;
	memcpy(&(st_send_node.st_destination), &(pst_recv_node->st_destination), sizeof(st_send_node.st_destination));
	memcpy(st_send_node.sock_path, pst_recv_node->sock_path, sizeof(pst_recv_node->sock_path));
	snprintf(st_send_node.msg_buf, BUF_LEN_1024, "%s", response);
	
    rtn = queue_add_node(&st_send_node, &g_st_send_queue);
	if (ACLK_ERROR == rtn){	
		return ACLK_ERROR;
	}

	return ACLK_OK;
}

static INT32 cmd_match(INT8 *cmd_line, const CMD_INTERFACE_T *pst_cmd_tbl)
{
	UINT32 i;
	
	if (cmd_line == NULL) {
		return ACLK_ERROR;
	}
		
	for(i=0; pst_cmd_tbl[i].cmd_head != NULL; i++) {
		if(strlen(pst_cmd_tbl[i].cmd_head) == strlen(cmd_line) && \
		   (strncasecmp(pst_cmd_tbl[i].cmd_head, cmd_line, strlen(cmd_line))==0)) {
			return i; 
		}
	}		
	
	return ACLK_ERROR;
}

INT32 node_format(QUEUE_NODE_T *pst_node)
{
	UINT32 i, j;
	UINT32 length;
	INT8 *data_buf;
	
    if (NULL == pst_node) {
       return ACLK_ERROR;
    }

	data_buf = pst_node->msg_buf;
	
	// trim heading white space
	for (i = 0; i < strlen(data_buf); i++) {
		if(!isspace(data_buf[i])) {
			break;
		}
	}
	data_buf = (data_buf + i);  // drop the white space

	// only keep one white space in the body
    length = strlen(data_buf); 
    for(i = 0; (i < length) && (i+1 < length); i++) {
        if(isspace(data_buf[i]) && isspace(data_buf[i+1])) {
            j=i; 
            for( ; j < length - 1; j++) {
                data_buf[j]=data_buf[j+1];
            }
            length--;
            i--;
            data_buf[length]=0;
        }
    }
     
    if(length > 1 && isspace(data_buf[length-1])) {
        data_buf[length-1] = '\0';
    }
	
	pst_node->msg_len = length;
	
    for(i=0; i < pst_node->msg_len; i++) {
        pst_node->msg_buf[i]= data_buf[i] ;
	}
	
    pst_node->msg_buf[i]= '\0';

	if (length == 0) {
		return ACLK_ERROR;  // empty line
	}
	
	return ACLK_OK;
}

static INT32 cmd_operation_type_parse(QUEUE_NODE_T *pst_node, UINT8 *operation_type)
{
	INT8 data_buf[BUF_LEN_1024];

	if (NULL == pst_node || NULL == operation_type) {
		return ACLK_ERROR;
	}
	
	// parse the operation type, set/show/showset
	if (strncasecmp(pst_node->msg_buf, __OID_SET__, PARA_OID_SET_HEADER_LEN) == 0) {
		*operation_type = OID_SET;	
		snprintf(data_buf, BUF_LEN_1024, "%s", pst_node->msg_buf + PARA_OID_SET_HEADER_LEN);
	} else if (strncasecmp(pst_node->msg_buf, __OID_SHOW__, PARA_OID_SHOW_HEADER_LEN) == 0) {
		*operation_type = OID_SHOW;	
		snprintf(data_buf, BUF_LEN_1024, "%s", pst_node->msg_buf + PARA_OID_SHOW_HEADER_LEN);
	} else if (strncasecmp(pst_node->msg_buf, __OID_SHOWSET__, PARA_OID_SHOWSET_HEADER_LEN) == 0) {
		*operation_type = OID_SHOWSET;	
		snprintf(data_buf, BUF_LEN_1024, "%s", pst_node->msg_buf + PARA_OID_SHOWSET_HEADER_LEN);
	} else {
		return ACLK_ERROR;
	}

	snprintf(pst_node->msg_buf, BUF_LEN_1024, "%s", data_buf);
	
	return ACLK_OK;
}


static INT32 cmd_oa_idx_parse(QUEUE_NODE_T *pst_node, UINT8 *oa_idx)
{
	INT8 data_buf[BUF_LEN_1024];
	INT8 slot;
	INT8 port;
	INT8 up_down_link;
	
	// 1. for amplifier command with "set/show/showset fe slot/port oa us/ds", parse it	
	if(isdigit(pst_node->msg_buf[0])) {		

		if ((pst_node->msg_buf[0] - '0') != SLOT1 && (pst_node->msg_buf[0] - '0') != SLOT2) {		
			return ACLK_ERROR;
		}
		slot = pst_node->msg_buf[0] - '0';


		// parse delimiter /
		if (pst_node->msg_buf[1] != '/') {
			return ACLK_ERROR;
		}
		

		if (!isdigit(pst_node->msg_buf[2]) || 
			((pst_node->msg_buf[2] - '0') != PORT1_NORMAL && (pst_node->msg_buf[2] - '0') != PORT2_REDUNDANT)) {
			return ACLK_ERROR;
		}
		port = pst_node->msg_buf[2] - '0';


		// check "OA us/ds" keyword
		if (strncasecmp(&(pst_node->msg_buf[4]), __OA_US_KEYWORD__, PARA_OA_KEYWORD_LEN) == 0) {
			up_down_link = OA_UPLINK;
		}  else if (strncasecmp(&(pst_node->msg_buf[4]), __OA_DS_KEYWORD__, PARA_OA_KEYWORD_LEN) == 0 ) {
			up_down_link = OA_DOWNLINK;
		} else {
			return ACLK_ERROR;
		}
		
		*oa_idx = (slot-1)*4 + (port-1)*2 + up_down_link;
		snprintf(data_buf, BUF_LEN_1024, "%s", (&pst_node->msg_buf[4] + PARA_OA_KEYWORD_LEN));
		snprintf(pst_node->msg_buf, BUF_LEN_1024, "%s", data_buf);
	} else {// 2. for board level command, oa index default to 0
		*oa_idx = 0;
	}
		
	return ACLK_OK;

}

// Due to Admire command format is fixed, it can send out command with "set/show/showset",
// we did some work to support Admire command for testing, only for testing compatibility
static INT32 cmd_admire_parse(const QUEUE_NODE_T *pst_node)
{
	const INT8 *pacCmdList[] = { "wm ", "rm ", "rdall" };
	INT32 i;

	if (NULL == pst_node) {
		return ACLK_ERROR;
	}
	
	for ( i = 0; i < sizeof(pacCmdList)/sizeof(pacCmdList[0]); i++) {
		if (strncasecmp(pst_node->msg_buf, pacCmdList[i], strlen(pacCmdList[i])) == 0) {		
			return ACLK_OK;
		}
	}
	return ACLK_ERROR;
}

// parse customer command, command begins with "set/show/showset"
static INT32 cmd_customer_parse(QUEUE_NODE_T *pst_node, UINT8 *oid_type, UINT8 *oa_idx, UINT8 *cmd_type)
{
	INT32 rtn;
	INT8 res_array[BUF_LEN_1024];

	if (NULL == pst_node || NULL == oid_type || NULL == oa_idx || NULL == cmd_type) {
		return ACLK_ERROR;
	}

	// check operation type, set/show/showset/else unknown type
	rtn = cmd_operation_type_parse(pst_node, oid_type);
	if (ACLK_ERROR == rtn) {
		snprintf(res_array, BUF_LEN_1024, INVALID_COMMAND_FORMAT, pst_node->msg_buf);	
		add_resp_2_send_queue(res_array, strlen(res_array), pst_node);
		return ACLK_ERROR;
	}

	// OA number parse
	rtn = cmd_oa_idx_parse(pst_node, oa_idx);
	if (ACLK_ERROR == rtn)
	{
		snprintf(res_array, BUF_LEN_1024, INVALID_COMMAND_FORMAT, pst_node->msg_buf);	
		add_resp_2_send_queue(res_array, strlen(res_array), pst_node);
		return ACLK_ERROR;
	}

	// command type parse
	if (*oa_idx != 0) {
		*cmd_type = CMD_OA;
	} else {
		*cmd_type = CMD_BOARD;
	}
	
	return ACLK_OK;
}


static INT32 msg_header_parse(QUEUE_NODE_T *pst_node, UINT8 *oid_type, UINT8 *oa_idx, UINT8 *cmd_type)
{
	if (NULL == pst_node || NULL == oid_type || NULL == oa_idx || NULL == cmd_type) {
		return ACLK_ERROR;
	}
	
	// check coming message from admire
	if (cmd_admire_parse(pst_node) == ACLK_OK) {
		*oid_type = OID_SHOW;    // admire command with no command header, raw asccii
		*oa_idx = 0;
		*cmd_type = CMD_BOARD;   // admire debug cmd all considered as board command
		return ACLK_OK;
	}

	// message coming from customer(uart, eth)
	return cmd_customer_parse(pst_node, oid_type, oa_idx, cmd_type);
}

INT32 node_depart(QUEUE_NODE_T *pst_node, const CMD_INTERFACE_T *pst_cmd_tbl)
{
	INT32 rtn;  
	INT32 oid;
    UINT8 para_num ;
	INT8 res_array[BUF_LEN_2048];  // queue node len 1024, buffer is 2048, for the case response msg len more than 1024
	INT8 *cmd_body[PARA_NUM_MAX+1];
	UINT8 cmd_type;
	UINT8 oid_type;
	UINT8 oa_num;
	INT8 *delim = "\t ";
	INT32 i;


    memset(cmd_body, 0, sizeof(cmd_body) );
	memset(res_array, 0, sizeof(res_array));

	// trim the message header
	rtn = msg_header_parse(pst_node, &oid_type, &oa_num, &cmd_type);
	if (ACLK_ERROR == rtn) {
		return ACLK_ERROR;
	}
	
	cmd_body[0] = strtok( pst_node->msg_buf, delim);

	para_num = 0 ;
	while(cmd_body[para_num] != NULL && para_num < PARA_NUM_MAX) {
		 para_num++;
         cmd_body[para_num] = strtok( NULL, delim);      	
	}

	oid = cmd_match(cmd_body[0], pst_cmd_tbl);
    if(oid < 0)  { // unknown command
		snprintf(res_array, BUF_LEN_1024, UNKNOWN_COMMAND, cmd_body[0]);	
        rtn = add_resp_2_send_queue(res_array, strlen(res_array), pst_node);
		if (ACLK_ERROR == rtn) {
			return ACLK_ERROR;
		}
    	return ACLK_OK;
    }

	// transform the command to lowcase
	for (i = 0; i < strlen(cmd_body[0]); i++) {
		cmd_body[0][i] = tolower(cmd_body[0][i]);
	}

	//execute the cmd and add response to response node
	memset(res_array, 0, sizeof(res_array));
	pst_cmd_tbl[oid].cmd_func(oid_type, oa_num, para_num, cmd_body, res_array, sizeof(res_array));

	// check if response message is over queue node size(1024)
	if (strlen(res_array) >= BUF_LEN_1024) {	
		for (i = 0; i < strlen(res_array)/BUF_LEN_1024; i++) {
			rtn = add_resp_2_send_queue(res_array+i*BUF_LEN_1024, BUF_LEN_1024, pst_node);
			if (ACLK_ERROR == rtn) {
				return ACLK_ERROR;
			}
		}

		if (strlen(res_array) % BUF_LEN_1024 != 0) {	
			rtn = add_resp_2_send_queue(res_array+i*BUF_LEN_1024, strlen(res_array) % BUF_LEN_1024, pst_node);
			if (ACLK_ERROR == rtn) {
				return ACLK_ERROR;
			}
		}
	} else {
		rtn = add_resp_2_send_queue(res_array, strlen(res_array), pst_node);
		if (ACLK_ERROR == rtn) {
			return ACLK_ERROR;
		}
	}
	
	return ACLK_OK;
}

