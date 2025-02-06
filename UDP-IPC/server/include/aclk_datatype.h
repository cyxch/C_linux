#ifndef _ACLK_DATATYPE_H_
#define _ACLK_DATATYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_LEN_8           (8)
#define BUF_LEN_32          (32)
#define BUF_LEN_64          (64)
#define BUF_LEN_128         (128)
#define BUF_LEN_256         (256)
#define BUF_LEN_512         (512)
#define BUF_LEN_1024        (1024)
#define BUF_LEN_2048        (2048)

#define ACLK_TRUE           (1)
#define ACLK_FALSE          (0)
#define ACLK_OK             (0)
#define ACLK_ERROR          (-1)
#define ACLK_NULL           (0)
#define ACLK_INCOMPLETE     (-2) 
#define ACLK_IGNORE			(-3) 
#define ACLK_HIS_CHAR		(-4) 
#define ACLK_BACKSPACE		(-5)
#define ACLK_TAB_CHAR		(-6)
#define ACLK_CMD_TIMEOUT	(-7)
#define ACLK_EMPTY_LINE		(-8)




#define SAFE_FREE(ptr) do { \
    if((ptr) != NULL) {     \
        free(ptr);          \
        (ptr) = NULL;       \
    }                       \
}while(0)

#define SAFE_FCLOSE(ptr) do { \
    if((ptr) != NULL) {       \
        fclose(ptr);          \
        (ptr) = NULL;         \
    }                         \
}while(0)

#define SAFE_CLOSE(fd) do { \
    if((fd) > 0) {          \
        close(fd);          \
        (fd) = -1;          \
    }                       \
}while(0)

typedef char            	INT8;
typedef short           	INT16;
typedef int             	INT32;
typedef long long           INT64;

typedef unsigned char   	UINT8;
typedef unsigned short  	UINT16;
typedef unsigned int    	UINT32;
typedef unsigned long long  UINT64;

typedef UINT32              BOOL;

typedef void            	VOID;



#ifdef __cplusplus
}
#endif

#endif                                                                  
