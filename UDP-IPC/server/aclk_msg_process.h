#ifndef __ACLK_MSG_PROCESS_H__
#define __ACLK_MSG_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aclk_datatype.h"


#define MAX_CONNECT_SOCKET  (10)

#define IPADDR_LOCAL       "127.0.0.1"


#define CLI_UDP_PORT_FOR_NET          (5320)


#define CLI_MSG_HEADER_LEN  (sizeof(MSG_HEADER_T))

/* command frame header                                                      */
typedef struct MSG_HEADER_t
{
    UINT16  msg_len;
    UINT16  usOpCode;
        
} __attribute__((packed)) MSG_HEADER_T;

/* command frame message                                                      */
typedef struct CMD_FRAME_t
{
    MSG_HEADER_T stMsgHeader;
    INT8 acRecBuf[BUF_LEN_64];
    
}__attribute__((packed)) CMD_FRAME_T;


/* response frame header                                                     */
typedef struct RSP_MSG_HEADER_t
{
    UINT16  msg_len;
    UINT8   ucCommandStatus;
        
}__attribute__((packed)) RSP_MSG_HEADER_T;

/* response frame message                                                     */
typedef struct RSP_FRAME_t
{
    RSP_MSG_HEADER_T stRspMsgHeader;
    INT8 cSendBuf[BUF_LEN_64];
    
}__attribute__((packed)) RSP_FRAME_T;


VOID srv_response_handler(VOID);
VOID srv_recv_msg_handler(VOID);
INT32 srv_entry(VOID);


#ifdef __cplusplus
}
#endif

#endif
