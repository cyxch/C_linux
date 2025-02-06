
#ifndef __ACLK_NODE_PROCESS_H__
#define __ACLK_NODE_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <strings.h>

#include "aclk_datatype.h"
#include "aclk_oid.h"
#include "aclk_queue.h"



#define	PARA_NUM_1      		1
#define PARA_NUM_2      		2
#define PARA_NUM_3      		3
#define PARA_NUM_4      		4
#define PARA_NUM_5      		5
#define PARA_NUM_6      		6

#define PARA_NUM_MAX			16


typedef INT32 (*PFCMDFUNC)( UINT8 oid_type, UINT8 oa_idx, 
						    UINT32 argc, INT8 *cmd_body[], 
						   INT8 *response, INT32 buf_len);


typedef struct CMD_INTERFACE_t
{
	INT8 *cmd_head;
	UINT32 cmd_agc_min;
	UINT32 cmd_agc_max;
	UINT8 cmd_attr;
    PFCMDFUNC cmd_func;
	const INT8 *cmd_desc;
}  CMD_INTERFACE_T;


// command type, board command or OA command
#define CMD_BOARD		(0x01 << 6)		//(bit 7)
#define CMD_OA			(0x01 << 7)		//(bit 8)

// operation type
#define OID_SHOW		0x01
#define	OID_SHOWSET		0x02
#define OID_SET			0x04


#define __OID_SET__						"set FE "
#define __OID_SHOW__					"show FE "
#define __OID_SHOWSET__					"showset FE "
#define __OA_US_KEYWORD__				"oa us "
#define __OA_DS_KEYWORD__				"oa ds "

#define PARA_OID_SET_HEADER_LEN			strlen(__OID_SET__)
#define PARA_OID_SHOW_HEADER_LEN		strlen(__OID_SHOW__)
#define PARA_OID_SHOWSET_HEADER_LEN		strlen(__OID_SHOWSET__)

#define PARA_OA_KEYWORD_LEN				strlen(__OA_US_KEYWORD__)

// echo chasis has 2 slots
typedef enum enSlotIndex
{
	SLOT1 = 1,
	SLOT2,
	SLOT_MAX
} SLOT_IDX_EN;

// echo slot has 2 ports
typedef enum enPortIndex
{
	PORT1_NORMAL = 1,
	PORT2_REDUNDANT,
	PORT_MAX
} PORT_IDX_EN;

// echo port has 2 OAs
typedef enum enLink
{
	OA_DOWNLINK = 1,
	OA_UPLINK,
} OA_LINK_EN;

typedef enum enOAIndex 
{
	CCU_BOARD = 0,
	OA_1,
	OA_2,
	OA_3,
	OA_4,
	OA_5,
	OA_6,
	OA_7,
	OA_8,
	OA_NUM_MAX
		
} OA_IDX_EN;


INT32 node_format(QUEUE_NODE_T * pst_node);
INT32 node_depart(QUEUE_NODE_T * pst_node, const CMD_INTERFACE_T *pst_cmd_tbl);



#ifdef __cplusplus
}
#endif

#endif


