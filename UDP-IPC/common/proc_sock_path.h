#ifndef __PROC_SOCK_PATH_H__
#define __PROC_SOCK_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <netinet/in.h>  
#include <sys/un.h>
#include "aclk_datatype.h"

// --- 日志打印封装 ---
// 定义日志级别 (可以根据需要扩展)
typedef enum {
    LOG_LEVEL_NONE = 0,     // 关闭日志
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,      // 更详细的调试信息
    LOG_LEVEL_ALL          // 打印所有级别日志
} LogLevel;


VOID log_print(LogLevel level, const INT8 *format, ...);

#define LOG_INFO(fmt, ...) \
    do {                     \
         log_print(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARNING(fmt, ...) \
    do {                       \
        log_print(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR(fmt, ...) \
    do {                     \
            log_print(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__); \
    } while (0)

#define LOG_DEBUG(fmt, ...) \
    do {                     \
            log_print(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__); \
    } while (0)




/* local sockets used in board IPC  */

#define PROC_LOCAL_SRV		"127.0.0.1"

// server unix socket path, used to IPC with ccu_dist/ccm
#define CLI_SOCK_PATH		("/tmp/cli_path")

// dist unix socket path, used to IPC with server
#define DIST_SOCK_PATH		("/usr/local/cfg/dist_path")

// ccm unix socket path, used to IPC with server
#define SRV_SOCK_PATH		("/tmp/ccm_path")


// cmd unix socket path, used to IPC with ccu_dist/server
#define CMD_SOCK_PATH		("/usr/local/tmp/cmd_path_XXXXXX")


// snmpd unix socket path, used to IPC with dist
#define SNMPD_SOCK_PATH		("/usr/local/cfg/snmpd_path")

#define __ERROR_PROMPT		"Error, "

#define __GOOGLE_PROMPT		"\r\n#"
#define __NUMBER_SIGN		"#"

#define __ID_CFG_FILE__		"%s.cfg"


// response code
#define INVALID_COMMAND_FORMAT			">%s:Error, Invalid command header format"__GOOGLE_PROMPT
#define OPERATION_NOT_ALLOWED			">%s:Error, Operation not allowed"__GOOGLE_PROMPT
#define UNKNOWN_COMMAND					">%s:Error, Unknown command"__GOOGLE_PROMPT
#define UNSUPPORTED						">%s:Error, Unsupported functionality"__GOOGLE_PROMPT
#define INVALID_ARGS					">%s:Error, Argument '%s' invalid"__GOOGLE_PROMPT
#define ARGS_NUM_INCORRECT				">%s:Error, Parameter number %d incorrect, within [%d, %d]"__GOOGLE_PROMPT
#define UNEXPECTED_ERROR				">%s:Error, Unexpected error"__GOOGLE_PROMPT
#define CMD_MISUSE						">%s:Error, Misuse wtih Amplifier Level Command and Module Level Command"__GOOGLE_PROMPT
#define TIME_OUT						">%s:Error, Response timeout"__GOOGLE_PROMPT
#define UPG_CRC_ERR						">%s:Error, Upgrade patch CRC error"__GOOGLE_PROMPT
#define OUT_OF_RANGE					">%s:Error, Argument '%s' out of range [%.2f, %.2f]"__GOOGLE_PROMPT
#define OA_NOT_EXISTED					">%s:Error, OA %d not exist"__GOOGLE_PROMPT
#define INVALID_SLOTorPORT				">%s:Error, Slot or Port not exist"__GOOGLE_PROMPT
#define OA_NOT_ALLOWED_CMD				">%s:Error, Upstream OA gain is fixed, not allowed to set"__GOOGLE_PROMPT
#define FPGA_WRorRD_ERROR				">%s:Error, FPGA write or read error, err-code:[0x%x-0x%x]"__GOOGLE_PROMPT

#define SOFT_RESET_OK					"Soft Reset:Ok"__GOOGLE_PROMPT
#define HARD_RESET_OK					"Hard Reset:Ok"__GOOGLE_PROMPT



VOID proc_srvctor(struct sockaddr_un * srv_addr, const INT8 * sock_path);
INT32 proc_create_unix_sock(const INT8 * sock_path);
INT32 proc_send_msg(INT32 cli_sock_fd, INT8 * command, INT32 length, INT8 * sock_path);



#ifdef __cplusplus
}
#endif

#endif
