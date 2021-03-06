/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "mpproto.h"
#include <rozofs/rozofs.h>

bool_t
xdr_mpp_status_t (XDR *xdrs, mpp_status_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mpp_status_ret_t (XDR *xdrs, mpp_status_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mpp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MPP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mpp_status_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_mpp_profiler_t (XDR *xdrs, mpp_profiler_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint64_t (xdrs, &objp->uptime))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->now))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->vers, 20,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_lookup, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_forget, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_getattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_setattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_readlink, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_mknod, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_mkdir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_unlink, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_rmdir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_symlink, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_rename, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_open, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_link, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_read, 3,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_write, 3,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_flush, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_release, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_opendir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_readdir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_releasedir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_fsyncdir, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_statfs, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_setxattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_getxattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_listxattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_removexattr, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_access, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_create, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_getlk, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_setlk, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->rozofs_ll_ioctl, 2,
		sizeof (uint64_t), (xdrproc_t) xdr_uint64_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mpp_profiler_ret_t (XDR *xdrs, mpp_profiler_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mpp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MPP_SUCCESS:
		 if (!xdr_mpp_profiler_t (xdrs, &objp->mpp_profiler_ret_t_u.profiler))
			 return FALSE;
		break;
	case MPP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mpp_profiler_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}
