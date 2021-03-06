/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SPPROTO_H_RPCGEN
#define _SPPROTO_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <rozofs/rozofs.h>

enum spp_status_t {
	SPP_SUCCESS = 0,
	SPP_FAILURE = 1,
};
typedef enum spp_status_t spp_status_t;

struct spp_status_ret_t {
	spp_status_t status;
	union {
		int error;
	} spp_status_ret_t_u;
};
typedef struct spp_status_ret_t spp_status_ret_t;

struct spp_profiler_t {
	uint64_t uptime;
	uint64_t now;
	uint8_t vers[20];
	uint64_t stat[2];
	uint64_t ports[2];
	uint64_t remove[2];
	uint64_t read[3];
	uint64_t write[3];
	uint64_t truncate[3];
	uint16_t nb_io_processes;
	uint16_t io_process_ports[32];
};
typedef struct spp_profiler_t spp_profiler_t;

struct spp_profiler_ret_t {
	spp_status_t status;
	union {
		spp_profiler_t profiler;
		int error;
	} spp_profiler_ret_t_u;
};
typedef struct spp_profiler_ret_t spp_profiler_ret_t;

#define STORAGED_PROFILE_PROGRAM 0x20000004
#define STORAGED_PROFILE_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define SPP_NULL 0
extern  void * spp_null_1(void *, CLIENT *);
extern  void * spp_null_1_svc(void *, struct svc_req *);
#define SPP_GET_PROFILER 1
extern  spp_profiler_ret_t * spp_get_profiler_1(void *, CLIENT *);
extern  spp_profiler_ret_t * spp_get_profiler_1_svc(void *, struct svc_req *);
#define SPP_CLEAR 2
extern  spp_status_ret_t * spp_clear_1(void *, CLIENT *);
extern  spp_status_ret_t * spp_clear_1_svc(void *, struct svc_req *);
extern int storaged_profile_program_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SPP_NULL 0
extern  void * spp_null_1();
extern  void * spp_null_1_svc();
#define SPP_GET_PROFILER 1
extern  spp_profiler_ret_t * spp_get_profiler_1();
extern  spp_profiler_ret_t * spp_get_profiler_1_svc();
#define SPP_CLEAR 2
extern  spp_status_ret_t * spp_clear_1();
extern  spp_status_ret_t * spp_clear_1_svc();
extern int storaged_profile_program_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_spp_status_t (XDR *, spp_status_t*);
extern  bool_t xdr_spp_status_ret_t (XDR *, spp_status_ret_t*);
extern  bool_t xdr_spp_profiler_t (XDR *, spp_profiler_t*);
extern  bool_t xdr_spp_profiler_ret_t (XDR *, spp_profiler_ret_t*);

#else /* K&R C */
extern bool_t xdr_spp_status_t ();
extern bool_t xdr_spp_status_ret_t ();
extern bool_t xdr_spp_profiler_t ();
extern bool_t xdr_spp_profiler_ret_t ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_SPPROTO_H_RPCGEN */
