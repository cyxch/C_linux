#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>



#include "aclk_datatype.h"
#include "client_common.h"
#include "proc_sock_path.h"

#define DIST_CFG	"/usr/local/sbin/dist.cfg"

static pthread_t g_udp_recv_thread_id;

static struct termios g_old_flags; 
struct termios g_new_flags;

#define DEFAULT_USER_NORMAL		"normal"
#define DEFAULT_USER_ADMIN		"superuser"

#define NORMAL_PW_EEPROM_ADDR		3200
#define ADMIN_PW_EEPROM_ADDR		3216
#define CLI_TIME_OUT_EEPROM_ADDR	3232

INT8 m_acCmdPath[BUF_LEN_32];
INT32 uart1_fd = STDIN_FILENO;


static VOID udp_recv_handler(VOID *arg)
{
	INT32 rtn;
	INT32 cli_sock_fd = -1;
	
    fd_set read_fds;
    struct timeval st_timeout;

    INT8 data_buf[BUF_LEN_1024];

	if (NULL == arg) {
		return;
	}
	cli_sock_fd = *(INT32 *)arg;

	while (1) {
		FD_ZERO (&read_fds);
		FD_SET (cli_sock_fd, &read_fds);
		st_timeout.tv_sec = 0;
		st_timeout.tv_usec = 0;
		
		rtn = select(cli_sock_fd + 1, &read_fds, NULL, NULL, &st_timeout);
		if (rtn <= 0) {
			continue;
		}
		
		if (FD_ISSET (cli_sock_fd, &read_fds)) {
			memset(data_buf, 0, sizeof(data_buf));
			rtn = recvfrom(cli_sock_fd, data_buf, BUF_LEN_1024, 0, NULL, NULL);
			if (rtn > 0) {
				rtn = write(uart1_fd, data_buf, sizeof(data_buf));
				if(rtn == -1) {
					LOG_ERROR("write failed");
				}
			}
		}
	}
	return;
}

INT32 main(INT32 argc, INT8 *argv[])
{
	INT32 cli_sock_fd = -1;
	INT32 rtn;	
    INT8 data_buf[BUF_LEN_256];
	INT8 *srv_path;
	//const INT8 *serial_port = "/dev/ttyPS1";
	
	#if 0
	uart1_fd = open(serial_port, O_RDWR);
	if(uart1_fd == -1) {
		LOG_ERROR("can't open uart1 \n");
	}
	#endif

	if (ACLK_OK == access(DIST_CFG, F_OK)) {
		srv_path = DIST_SOCK_PATH; 
	} else {
		srv_path = SRV_SOCK_PATH;
	}
	
	cli_sock_fd = proc_create_unix_sock(CLI_SOCK_PATH);		
	if (cli_sock_fd < 0) {
		LOG_ERROR("server: create sock error\n");
		return ACLK_ERROR;
	}
	
	// thread to recv udp socket data
	rtn = pthread_create(&g_udp_recv_thread_id, NULL, (VOID*)udp_recv_handler, &cli_sock_fd);
	if (ACLK_OK != rtn) {
		LOG_ERROR("server: create recv error\n");
		return ACLK_ERROR;
	}

	// set stdin to non-ICANON mode
	cmd_get_fd_attr(uart1_fd, &g_old_flags);
	cmd_set_fd_attr(uart1_fd, &g_old_flags, &g_new_flags);


	// main thread to read data from stdio, and send to server
	while (1) {
		memset(data_buf, 0, BUF_LEN_256);
		usleep(10000);
		rtn = cmd_read_line(data_buf, BUF_LEN_256);
		if (rtn == ACLK_ERROR || rtn == ACLK_EMPTY_LINE) {		
		    if (rtn == ACLK_EMPTY_LINE) {
				if (data_buf[0] == 0) {
					LOG_ERROR("%s", __PROMPT__);
				} else {
					LOG_ERROR("\n%s", __PROMPT__);
				}
			}
			continue;
		}		

		
		cmd_history_update(&g_st_history_list, data_buf, strlen(data_buf));
		
		if (strcasecmp(data_buf, "quit") == 0 || strcasecmp(data_buf, "exit") == 0 ) {					
			tcsetattr(uart1_fd, TCSANOW, &g_old_flags);
			return 0;
		}	
	
		
		rtn = proc_send_msg(cli_sock_fd, data_buf, strlen(data_buf), srv_path);	
		if (strlen(data_buf) != rtn) {
			LOG_ERROR("server:Error: failed to send data [%s] to server, rtn = %d\n", data_buf, rtn);  
			continue;
		}
	}
	return 0;
}

