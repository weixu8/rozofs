/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SPROTO_H_RPCGEN
#define _SPROTO_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "rozofs.h"

    typedef u_char sp_uuid_t[ROZOFS_UUID_SIZE];

    enum sp_status_t {
        SP_SUCCESS = 0,
        SP_FAILURE = 1,
    };
    typedef enum sp_status_t sp_status_t;

    struct sp_status_ret_t {
        sp_status_t status;
        union {
            int error;
        } sp_status_ret_t_u;
    };
    typedef struct sp_status_ret_t sp_status_ret_t;

    struct sp_remove_arg_t {
        uint16_t sid;
        sp_uuid_t fid;
    };
    typedef struct sp_remove_arg_t sp_remove_arg_t;

    struct sp_write_arg_t {
        uint16_t sid;
        sp_uuid_t fid;
        uint8_t tid;
        uint64_t bid;
        uint32_t nrb;
        struct {
            u_int bins_len;
            char *bins_val;
        } bins;
    };
    typedef struct sp_write_arg_t sp_write_arg_t;

    struct sp_read_arg_t {
        uint16_t sid;
        sp_uuid_t fid;
        uint8_t tid;
        uint64_t bid;
        uint32_t nrb;
    };
    typedef struct sp_read_arg_t sp_read_arg_t;

    struct sp_truncate_arg_t {
        uint16_t sid;
        sp_uuid_t fid;
        uint8_t tid;
        uint64_t bid;
    };
    typedef struct sp_truncate_arg_t sp_truncate_arg_t;

    struct sp_read_ret_t {
        sp_status_t status;
        union {
            struct {
                u_int bins_len;
                char *bins_val;
            } bins;
            int error;
        } sp_read_ret_t_u;
    };
    typedef struct sp_read_ret_t sp_read_ret_t;

    struct sp_sstat_t {
        uint64_t size;
        uint64_t free;
    };
    typedef struct sp_sstat_t sp_sstat_t;

    struct sp_stat_ret_t {
        sp_status_t status;
        union {
            sp_sstat_t sstat;
            int error;
        } sp_stat_ret_t_u;
    };
    typedef struct sp_stat_ret_t sp_stat_ret_t;

#define STORAGE_PROGRAM 0x20000002
#define STORAGE_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define SP_NULL 0
    extern void *sp_null_1(void *, CLIENT *);
    extern void *sp_null_1_svc(void *, struct svc_req *);
#define SP_REMOVE 1
    extern sp_status_ret_t *sp_remove_1(sp_remove_arg_t *, CLIENT *);
    extern sp_status_ret_t *sp_remove_1_svc(sp_remove_arg_t *,
                                            struct svc_req *);
#define SP_WRITE 2
    extern sp_status_ret_t *sp_write_1(sp_write_arg_t *, CLIENT *);
    extern sp_status_ret_t *sp_write_1_svc(sp_write_arg_t *,
                                           struct svc_req *);
#define SP_READ 3
    extern sp_read_ret_t *sp_read_1(sp_read_arg_t *, CLIENT *);
    extern sp_read_ret_t *sp_read_1_svc(sp_read_arg_t *, struct svc_req *);
#define SP_TRUNCATE 4
    extern sp_status_ret_t *sp_truncate_1(sp_truncate_arg_t *, CLIENT *);
    extern sp_status_ret_t *sp_truncate_1_svc(sp_truncate_arg_t *,
                                              struct svc_req *);
#define SP_STAT 5
    extern sp_stat_ret_t *sp_stat_1(uint16_t *, CLIENT *);
    extern sp_stat_ret_t *sp_stat_1_svc(uint16_t *, struct svc_req *);
    extern int storage_program_1_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

#else                           /* K&R C */
#define SP_NULL 0
    extern void *sp_null_1();
    extern void *sp_null_1_svc();
#define SP_REMOVE 1
    extern sp_status_ret_t *sp_remove_1();
    extern sp_status_ret_t *sp_remove_1_svc();
#define SP_WRITE 2
    extern sp_status_ret_t *sp_write_1();
    extern sp_status_ret_t *sp_write_1_svc();
#define SP_READ 3
    extern sp_read_ret_t *sp_read_1();
    extern sp_read_ret_t *sp_read_1_svc();
#define SP_TRUNCATE 4
    extern sp_status_ret_t *sp_truncate_1();
    extern sp_status_ret_t *sp_truncate_1_svc();
#define SP_STAT 5
    extern sp_stat_ret_t *sp_stat_1();
    extern sp_stat_ret_t *sp_stat_1_svc();
    extern int storage_program_1_freeresult();
#endif                          /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
    extern bool_t xdr_sp_uuid_t(XDR *, sp_uuid_t);
    extern bool_t xdr_sp_status_t(XDR *, sp_status_t *);
    extern bool_t xdr_sp_status_ret_t(XDR *, sp_status_ret_t *);
    extern bool_t xdr_sp_remove_arg_t(XDR *, sp_remove_arg_t *);
    extern bool_t xdr_sp_write_arg_t(XDR *, sp_write_arg_t *);
    extern bool_t xdr_sp_read_arg_t(XDR *, sp_read_arg_t *);
    extern bool_t xdr_sp_truncate_arg_t(XDR *, sp_truncate_arg_t *);
    extern bool_t xdr_sp_read_ret_t(XDR *, sp_read_ret_t *);
    extern bool_t xdr_sp_sstat_t(XDR *, sp_sstat_t *);
    extern bool_t xdr_sp_stat_ret_t(XDR *, sp_stat_ret_t *);

#else                           /* K&R C */
    extern bool_t xdr_sp_uuid_t();
    extern bool_t xdr_sp_status_t();
    extern bool_t xdr_sp_status_ret_t();
    extern bool_t xdr_sp_remove_arg_t();
    extern bool_t xdr_sp_write_arg_t();
    extern bool_t xdr_sp_read_arg_t();
    extern bool_t xdr_sp_truncate_arg_t();
    extern bool_t xdr_sp_read_ret_t();
    extern bool_t xdr_sp_sstat_t();
    extern bool_t xdr_sp_stat_ret_t();

#endif                          /* K&R C */

#ifdef __cplusplus
}
#endif
#endif                          /* !_SPROTO_H_RPCGEN */
