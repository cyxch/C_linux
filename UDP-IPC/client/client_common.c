#include <stdio.h>
#include <sys/types.h>			
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/un.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

#include "proc_sock_path.h"
#include "aclk_datatype.h"
#include "client_common.h"

HISTORY_CMD_T g_st_history_list;
extern INT32 uart1_fd;



// dynamically update the history command list when user input command
VOID cmd_history_update(HISTORY_CMD_T * pstHistoryList, INT8 *pcCmd, INT32 length)
{
	// if reach the history number limit, come from beginning
    if (pstHistoryList->ucHisTail == HIST_NUM_MAX)
    {
        pstHistoryList->ucHisTail = 0;
    }

	// store the new coming cmd
	snprintf(pstHistoryList->acHistory[pstHistoryList->ucHisTail], BUF_LEN_256 -1, "%s", pcCmd);
	
    pstHistoryList->ucHisTail++;

	// history cmd number ++, util it reach the max number, then keep it always as HIST_NUM_MAX
    if (pstHistoryList->ucHisNums < HIST_NUM_MAX)
    {
        pstHistoryList->ucHisNums++;
    }

    pstHistoryList->ucCurHisIndex = pstHistoryList->ucHisTail;

}


static VOID cmd_backspace()
{
	INT8 tmp_char;
	INT32 rtn;
	
	tmp_char = '\b';
    rtn = write(STDIN_FILENO, &tmp_char, 1);
	if(rtn == -1) {
		LOG_ERROR("write failed");
	}
    
	tmp_char = ' ';
    rtn = write(STDIN_FILENO, &tmp_char, 1);
	if(rtn == -1) {
		LOG_ERROR("write failed");
	}
	
	tmp_char = '\b';
    rtn =write(STDIN_FILENO, &tmp_char, 1);
	if(rtn == -1) {
		LOG_ERROR("write failed");
	}
}



static VOID cmd_del_tab_char(INT8 *data_buf)
{
    INT32 i = 0;
	INT32 length;

	if ((g_new_flags.c_lflag & ECHO) == 0) { // if echo off, no need to delete data, since there is on display
		return;
	}
	
	length = ((data_buf[0] == 0) ? HEAD_TAB_LEN : MID_TAB_LEN);

    for (i = 0; i < length; i++) {
        cmd_backspace();
    }

    return;
}


static INT32 cmd_parse_line(INT8 tmp_data, INT8 *data_buf)
{
	static BOOL is_last_char = ACLK_FALSE;
	INT32 i;
	
	if (tmp_data == '\r') {
		is_last_char = ACLK_TRUE;
		if (data_buf[0] == 0) {	
			return ACLK_EMPTY_LINE;
		}
		return ACLK_OK;
	}
	
	if (tmp_data == '\n') {	
		if (data_buf[0] == 0) {
			if (is_last_char == ACLK_FALSE) {
				return ACLK_EMPTY_LINE;
			}
			
			is_last_char = ACLK_FALSE;
			return ACLK_IGNORE;  // if last character is 0x0d, ignore the following 0x0a
		}
		
		is_last_char = ACLK_FALSE;
		return ACLK_OK;  // nornal terminated line
	}

	if (tmp_data == '\t') {
		cmd_del_tab_char(data_buf);
		return ACLK_TAB_CHAR;
	}

	is_last_char = ACLK_FALSE;

	if ('\b' == tmp_data) {   // '\b'=0x08, but display as '^H', two characters		
		if ((g_new_flags.c_lflag & ECHO) != 0) { // if echo on, need to delete the ^H, else no need
			for ( i = 0; i < BACKSPACE_LEN; i++) {
				cmd_backspace();	
			}

			if (data_buf[0] != 0 ) { // if input, plus the deleted character, so len is BACKSPACE_LEN + 1
				cmd_backspace();	
			}
		}
		return ACLK_BACKSPACE;
	}
	return ACLK_INCOMPLETE;
}

static INT32 cmd_read_char(INT32 cli_sock_fd, INT8 *recv_buf, UINT64 length)
{
    INT32 rtn;

    if (NULL == recv_buf ) {
        return ACLK_ERROR;
    }

    rtn = read(cli_sock_fd, recv_buf, length);
    if (rtn <= 0) {
        return ACLK_ERROR;
    }

    return rtn;
}

INT32 cmd_read_line(INT8 *data_buf, INT32 length)
{
	INT32 tmp_len;
	INT8 tmp_data;
	INT32 rtn;

	
	if (NULL == data_buf) {
		LOG_INFO("Please Input The Command\n");
		return ACLK_ERROR;
	}	

	while(1) {
		tmp_len = cmd_read_char(uart1_fd, &tmp_data, 1);		
		if(tmp_len == ACLK_ERROR) {			
			continue;
		}
	
		rtn = cmd_parse_line(tmp_data, data_buf);
		if (rtn == ACLK_EMPTY_LINE || rtn == ACLK_OK) {
			return rtn;
		} else if (rtn == ACLK_BACKSPACE) {
			if (data_buf[0] != 0) {
				tmp_len = strlen(data_buf);
				if (tmp_len >= 1 && tmp_len < length) {
					data_buf[tmp_len-1] = 0;
				}
			}
		} else if (rtn == ACLK_INCOMPLETE) {
			tmp_len = strlen(data_buf);
			if (tmp_len >= length-1) {
				LOG_INFO("Input command exceed max limit %d, please re-input again\n", length);
				return ACLK_ERROR;
			}		
			data_buf[tmp_len] = tmp_data;
		}	

	}	
				 
	return ACLK_OK;
}


INT32 cmd_get_fd_attr(INT32 cli_sock_fd, struct termios *pst_curr_flags)
{
	if (NULL == pst_curr_flags) {
		return ACLK_ERROR;
	}
	
	tcgetattr(cli_sock_fd, pst_curr_flags);
	
	return ACLK_OK;
}


INT32 cmd_set_fd_attr(INT32 cli_sock_fd, struct termios *pst_old_flags, struct termios *pst_new_flags)
{
    struct termios new_flags;

	if (NULL == pst_old_flags || NULL == pst_new_flags) {
		return ACLK_ERROR;
	}

    new_flags = *pst_old_flags;

    memset(&new_flags.c_cc, 0x00, sizeof(new_flags.c_cc));

    new_flags.c_lflag &= ~(ICANON | ISIG);  // default echo on
    
    new_flags.c_iflag &= ~(BRKINT );
	new_flags.c_iflag	&= ~INLCR;	  // not transfer NL as CR
	new_flags.c_iflag	|= ICRNL;	  // transfer CR as NL

    new_flags.c_cc[VTIME] = 0;
    new_flags.c_cc[VMIN] = 1;

	new_flags.c_iflag = 0x500;
	new_flags.c_lflag = 0x8a38;

    if(tcsetattr(cli_sock_fd, TCSAFLUSH, &new_flags) < 0) {
		LOG_ERROR("%s\n", "fail to set term attribute");
		tcsetattr(cli_sock_fd, TCSANOW, pst_old_flags);
		return ACLK_ERROR;
	}

	*pst_new_flags = new_flags;

	return ACLK_OK;
}




INT32 UartBaudRateSet(UINT32 uiBaudrate)
{
    struct termios stTermAttr;
	
    if (BAUDRATE_9600 != uiBaudrate && BAUDRATE_19200 != uiBaudrate && 
		BAUDRATE_38400 != uiBaudrate && BAUDRATE_57600 != uiBaudrate && 
		BAUDRATE_115200 != uiBaudrate)
    {
        //ACLK_LogDump(ACLK_ERR, __FILE__, __LINE__, "get baudrate error!");
		return ACLK_ERROR;
    }

	// get terminal attribute
    tcgetattr(STDIN_FILENO, &stTermAttr);

	// set baudrate
    if (BAUDRATE_9600 == uiBaudrate)
    {
        cfsetispeed(&stTermAttr, B9600);
    }
    else if (BAUDRATE_19200 == uiBaudrate)
    {
        cfsetispeed(&stTermAttr, B19200);
    }
    else if (BAUDRATE_38400 == uiBaudrate)
    {
        cfsetispeed(&stTermAttr, B38400);
    }
    else if (BAUDRATE_57600 == uiBaudrate)
    {
        cfsetispeed(&stTermAttr, B57600);
    }
    else
    {
        cfsetispeed(&stTermAttr, B115200);
    }

	// set terminal attribute
    tcsetattr(STDIN_FILENO, TCSANOW, &stTermAttr);

	return ACLK_OK;
}

