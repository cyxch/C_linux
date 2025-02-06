#ifndef __CLIENT_COMMON_H__
#define __CLIENT_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>

#include "aclk_datatype.h"

#define GOOGLE_PROMPT	"#"
#define NORMAL_PROMPT	"$"

#define __PROMPT__	GOOGLE_PROMPT



#define ARROW_LEN		4  // ^[[A ^[[B  ^[[C ^[[D
#define BACKSPACE_LEN   2  // ^H
#define HEAD_TAB_LEN	7  // the first table character in a line "	"
#define MID_TAB_LEN		5  // table character in the middle of a line, "ab	cd"

typedef enum enBaudRate
{
	BAUDRATE_9600	= 9600,
	BAUDRATE_19200	= 19200,
	BAUDRATE_38400	= 38400,
	BAUDRATE_57600	= 57600,
	BAUDRATE_115200	= 115200,
	
} EN_BAUDRATE;

#define __CMD_CONSOLE__		"/dev/console"
#define __CMD_BAUDRATE__	"/usr/local/cfg/cmd_baudrate"

#define HIST_NUM_MAX		10

// structure hold the history command
typedef struct stHistoryCmd
{
	INT8	 acHistory[HIST_NUM_MAX][BUF_LEN_256];  // history command storage
	UINT8	 ucHisNums;		   						// command numbers
	UINT8	 ucHisTail;        						// always pointer to last command
	UINT8	 ucCurHisIndex;	   						// current history comand
} HISTORY_CMD_T;

// prompt sign 
#define GOOGLE_PROMPT	"#"
#define NORMAL_PROMPT	"$"


#define CLI_TIME_OUT_DFT		(5)
#define CLI_TIME_OUT_MAX		(71582788)   // 0xFFFFFFFF/60


INT32 cmd_read_line(INT8 *data_buf, INT32 length);
INT32 cmd_get_fd_attr(INT32 cli_sock_fd, struct termios *pst_curr_flags);
INT32 cmd_set_fd_attr(INT32 cli_sock_fd, struct termios *pst_old_flags, struct termios *pst_new_flags);

VOID cmd_history_update(HISTORY_CMD_T * pstHistoryList, INT8 *pcCmd, INT32 length);



extern struct termios g_new_flags;

extern HISTORY_CMD_T g_st_history_list;


#ifdef __cplusplus
}
#endif

#endif
