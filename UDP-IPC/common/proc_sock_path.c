#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>  
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>      // 文件控制定义 (O_RDWR 等)
#include <unistd.h>     // POSIX 操作系统 API (close, perror 等)
#include <errno.h>      // 错误号定义 (errno)
#include <string.h>     // 字符串操作 (strerror)
#include <stdarg.h>     // 可变参数列表 (用于日志函数)
#include <time.h>       // 时间函数 (time, strftime)

#include "aclk_datatype.h"
#include "proc_sock_path.h"

// 日志级别字符串数组，方便打印
const INT8* log_level_strings[] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "VERBOSE",
    "ALL"
};

// 当前日志级别 (全局变量，可以在程序中设置)
LogLevel current_log_level = LOG_LEVEL_INFO; // 默认设置为 INFO 级别

// 设置日志级别的函数
VOID set_log_level(LogLevel level) {
    current_log_level = level;
    log_print(LOG_LEVEL_INFO, "日志级别已设置为: %s", log_level_strings[level]);
}

// 获取当前时间字符串 (用于日志时间戳)
const INT8* get_current_timestamp() {
    static INT8 timestamp_str[30]; // 静态缓冲区，避免多次分配内存
    time_t timer;
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer); // 使用 localtime 获取本地时间

    strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d %H:%M:%S", tm_info);
    return timestamp_str;
}


// 日志打印函数 (可变参数)
VOID log_print(LogLevel level, const INT8 *format, ...) {
    if (level > current_log_level || level == LOG_LEVEL_NONE) return; // 检查日志级别，不符合则直接返回

    fprintf(stderr, "[%s] [%s] [%s:%d] ", // 打印时间戳，日志级别，函数名:行号
            get_current_timestamp(),
            log_level_strings[level],
            __FUNCTION__,       // 打印函数名
            __LINE__           // 打印行号
           );

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args); // 使用 vfprintf 打印可变参数
    va_end(args);
    fprintf(stderr, "\n"); // 换行
}



VOID proc_srvctor(struct sockaddr_un *srv_addr, const INT8 *sock_path)
{
    if (NULL == srv_addr || NULL == sock_path) {
        return;
    }

    memset(srv_addr, 0, sizeof(struct sockaddr_un));
    srv_addr->sun_family = AF_UNIX;
	strncpy(srv_addr->sun_path, sock_path, sizeof(srv_addr->sun_path) - 1);
}


INT32 proc_create_unix_sock(const INT8 *sock_path)
{
	INT32 cli_sock_fd; 
	struct sockaddr_un sock_addr;
	INT32 length;

	if (NULL == sock_path) {
		goto ERR_LABEL;
	}

	proc_srvctor(&sock_addr, sock_path);

	cli_sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (cli_sock_fd < 0) {
		LOG_ERROR("create unix socket error\n");
		goto ERR_LABEL;
	}

	unlink(sock_addr.sun_path);
	length = offsetof(struct sockaddr_un, sun_path) + strlen(sock_addr.sun_path);   
	if (bind(cli_sock_fd, (struct sockaddr *)&sock_addr, length) < 0 ) {
		LOG_ERROR("bind unix socket %s error\n", sock_addr.sun_path);
		goto ERR_LABEL;
	}

	return cli_sock_fd;

ERR_LABEL:

    return -1;
}


INT32 proc_send_msg(INT32 cli_sock_fd, INT8 *command, INT32 length, INT8 *sock_path)
{
	INT32 rtn;	
    INT32 tmp_len;
	struct sockaddr_un srv_addr;

	if (cli_sock_fd < 0 || NULL ==	command) {
		return -1;
	}

	proc_srvctor(&srv_addr, sock_path); 

    tmp_len = offsetof(struct sockaddr_un, sun_path) + strlen(srv_addr.sun_path);
	
	rtn = sendto(cli_sock_fd, command, length, 0, (struct sockaddr*)&srv_addr, tmp_len);
	
	return rtn;
}

