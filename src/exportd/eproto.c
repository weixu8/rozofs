/*
 Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
 This file is part of Rozofs.

 Rozofs is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by the Free Software Foundation, version 2.

 Rozofs is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see
 <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include <rozofs/common/log.h>
#include <rozofs/common/xmalloc.h>
#include <rozofs/common/profile.h>
#include <rozofs/rpc/epproto.h>
#include <rozofs/rpc/eproto.h>
#include <rozofs/rpc/sproto.h>

#include "export.h"
#include "volume.h"
#include "exportd.h"

DECLARE_PROFILING(epp_profiler_t);

void *ep_null_1_svc(void *noargs, struct svc_req *req) {
    DEBUG_FUNCTION;
    return 0;
}

ep_mount_ret_t *ep_mount_1_svc(ep_path_t * arg, struct svc_req * req) {
    static ep_mount_ret_t ret;
    list_t *p, *q, *r;
    eid_t *eid = NULL;
    export_t *exp;
    int i = 0;
    int stor_idx = 0;
    int exist = 0;

    DEBUG_FUNCTION;
    START_PROFILING(ep_mount);

    // XXX exportd_lookup_id could return export_t *
    if (!(eid = exports_lookup_id(*arg)))
        goto error;
    if (!(exp = exports_lookup_export(*eid)))
        goto error;

    /* Get lock on config */
    if ((errno = pthread_rwlock_rdlock(&config_lock)) != 0) {
        goto error;
    }

    /* For each volume */
    list_for_each_forward(p, &exportd_config.volumes) {

        volume_config_t *vc = list_entry(p, volume_config_t, list);

        /* Get volume with this vid */
        if (vc->vid == exp->volume->vid) {

            stor_idx = 0;
            ret.ep_mount_ret_t_u.export.storage_nodes_nb = 0;
            memset(ret.ep_mount_ret_t_u.export.storage_nodes, 0, sizeof (ep_storage_node_t) * ROZOFS_STORAGES_MAX);

            /* For each cluster */
            list_for_each_forward(q, &vc->clusters) {

                cluster_config_t *cc = list_entry(q, cluster_config_t, list);

                /* For each sid */
                list_for_each_forward(r, &cc->storages) {

                    storage_node_config_t *s = list_entry(r, storage_node_config_t, list);

                    /* Verify that this hostname does not already exist
                     * in the list of physical storage nodes. */
                    for (i = 0; i < stor_idx; i++) {

                        if (strcmp(s->host, ret.ep_mount_ret_t_u.export.storage_nodes[i].host) == 0) {

                            /* This physical storage node exist
                             *  but we add this SID*/
                            uint8_t sids_nb = ret.ep_mount_ret_t_u.export.storage_nodes[i].sids_nb;
                            ret.ep_mount_ret_t_u.export.storage_nodes[i].sids[sids_nb] = s->sid;
                            ret.ep_mount_ret_t_u.export.storage_nodes[i].sids_nb++;
                            exist = 1;
                            break;
                        }
                    }

                    /* This physical storage node doesn't exist*/
                    if (exist == 0) {

                        /* Add this storage node to the list */
                        strcpy(ret.ep_mount_ret_t_u.export.storage_nodes[stor_idx].host, s->host);
                        /* Add this sid */
                        ret.ep_mount_ret_t_u.export.storage_nodes[stor_idx].sids[0] = s->sid;
                        ret.ep_mount_ret_t_u.export.storage_nodes[stor_idx].sids_nb++;

                        /* Increments the nb. of physical storage nodes */
                        stor_idx++;
                    }
                    exist = 0;
                }
            }
        }
    }

    ret.ep_mount_ret_t_u.export.storage_nodes_nb = stor_idx;
    ret.ep_mount_ret_t_u.export.eid = *eid;
    memcpy(ret.ep_mount_ret_t_u.export.md5, exp->md5, ROZOFS_MD5_SIZE);
    ret.ep_mount_ret_t_u.export.rl = exportd_config.layout;
    memcpy(ret.ep_mount_ret_t_u.export.rfid, exp->rfid, sizeof (fid_t));

    if ((errno = pthread_rwlock_unlock(&config_lock)) != 0) {
        goto error;
    }

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mount_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_mount);
    return &ret;
}

/* Will do something one day !! */
ep_status_ret_t *ep_umount_1_svc(uint32_t * arg, struct svc_req * req) {
    static ep_status_ret_t ret;
    DEBUG_FUNCTION;
    START_PROFILING(ep_umount);

    ret.status = EP_SUCCESS;

    STOP_PROFILING(ep_umount);
    return &ret;
}

ep_statfs_ret_t *ep_statfs_1_svc(uint32_t * arg, struct svc_req * req) {
    static ep_statfs_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_statfs);

    if (!(exp = exports_lookup_export((eid_t) * arg)))
        goto error;
    if (export_stat(exp, (estat_t *) & ret.ep_statfs_ret_t_u.stat) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_statfs_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_statfs);
    return &ret;
}

ep_mattr_ret_t *ep_lookup_1_svc(ep_lookup_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_lookup);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_lookup
            (exp, arg->parent, arg->name,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_lookup);
    return &ret;
}

ep_mattr_ret_t *ep_getattr_1_svc(ep_mfile_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_getattr);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_getattr
            (exp, arg->fid, (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_getattr);
    return &ret;
}

ep_mattr_ret_t *ep_setattr_1_svc(ep_setattr_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_setattr);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_setattr(exp, arg->attrs.fid, (mattr_t *) & arg->attrs,
            arg->to_set) != 0)
        goto error;
    if (export_getattr(exp, arg->attrs.fid,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_setattr);
    return &ret;
}

ep_readlink_ret_t *ep_readlink_1_svc(ep_mfile_arg_t * arg,
        struct svc_req * req) {
    static ep_readlink_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_readlink);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    ret.ep_readlink_ret_t_u.link = xmalloc(ROZOFS_PATH_MAX);
    if (export_readlink(exp, arg->fid, ret.ep_readlink_ret_t_u.link) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_readlink_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_readlink);
    return &ret;
}

ep_mattr_ret_t *ep_link_1_svc(ep_link_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_link);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_link(exp, arg->inode, arg->newparent, arg->newname,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_link);
    return &ret;
}

ep_mattr_ret_t *ep_mknod_1_svc(ep_mknod_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_mknod);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_mknod
            (exp, arg->parent, arg->name, arg->uid, arg->gid, arg->mode,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_mknod);
    return &ret;
}

ep_mattr_ret_t *ep_mkdir_1_svc(ep_mkdir_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_mkdir);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_mkdir
            (exp, arg->parent, arg->name, arg->uid, arg->gid, arg->mode,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_mkdir);
    return &ret;
}

ep_fid_ret_t *ep_unlink_1_svc(ep_unlink_arg_t * arg, struct svc_req * req) {
    static ep_fid_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_unlink);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_unlink(exp, arg->pfid, arg->name, ret.ep_fid_ret_t_u.fid) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_fid_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_unlink);
    return &ret;
}

ep_fid_ret_t *ep_rmdir_1_svc(ep_rmdir_arg_t * arg, struct svc_req * req) {
    static ep_fid_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_rmdir);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_rmdir(exp, arg->pfid, arg->name, ret.ep_fid_ret_t_u.fid) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_fid_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_rmdir);
    return &ret;
}

ep_mattr_ret_t *ep_symlink_1_svc(ep_symlink_arg_t * arg, struct svc_req * req) {
    static ep_mattr_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_symlink);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_symlink(exp, arg->link, arg->parent, arg->name,
            (mattr_t *) & ret.ep_mattr_ret_t_u.attrs) != 0)
        goto error;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_mattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_symlink);
    return &ret;
}

ep_fid_ret_t *ep_rename_1_svc(ep_rename_arg_t * arg, struct svc_req * req) {
    static ep_fid_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_rename);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if (export_rename(exp, arg->pfid, arg->name, arg->npfid, arg->newname,
            ret.ep_fid_ret_t_u.fid) != 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_fid_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_rename);
    return &ret;
}

ep_readdir_ret_t *ep_readdir_1_svc(ep_readdir_arg_t * arg,
        struct svc_req * req) {
    static ep_readdir_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING(ep_readdir);

    xdr_free((xdrproc_t) xdr_ep_readdir_ret_t, (char *) &ret);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_readdir(exp, arg->fid, &arg->cookie, (child_t **) & ret.ep_readdir_ret_t_u.reply.children, (uint8_t *) & ret.ep_readdir_ret_t_u.reply.eof) != 0)
        goto error;

    ret.ep_readdir_ret_t_u.reply.cookie = arg->cookie;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_readdir_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_readdir);
    return &ret;
}

/* not used anymore
ep_io_ret_t *ep_read_1_svc(ep_io_arg_t * arg, struct svc_req * req) {
    static ep_io_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if ((ret.ep_io_ret_t_u.length =
            export_read(exp, arg->fid, arg->offset, arg->length)) < 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_io_ret_t_u.error = errno;
out:
    return &ret;
}
 */

ep_read_block_ret_t *ep_read_block_1_svc(ep_io_arg_t * arg, struct svc_req * req) {
    static ep_read_block_ret_t ret;
    export_t *exp = NULL;
    int64_t length = -1;
    uint64_t first_blk = 0;
    uint32_t nb_blks = 0;

    DEBUG_FUNCTION;
    START_PROFILING_IO(ep_read_block, arg->length);

    // Free memory buffers for xdr
    xdr_free((xdrproc_t) xdr_ep_read_block_ret_t, (char *) &ret);

    // Get export
    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    // Check if EOF, get nb. of blocks to read and update atime
    if ((length = export_read(exp, arg->fid, arg->offset, arg->length, &first_blk, &nb_blks)) == -1)
        goto error;

    ret.ep_read_block_ret_t_u.ret.length = length;
    ret.ep_read_block_ret_t_u.ret.dist.dist_len = nb_blks;
    ret.ep_read_block_ret_t_u.ret.dist.dist_val = xmalloc(nb_blks * sizeof (dist_t));

    // Get distributions
    if (export_read_block(exp, arg->fid, first_blk, nb_blks, ret.ep_read_block_ret_t_u.ret.dist.dist_val) != 0)
        goto error;

    ret.status = EP_SUCCESS;
    goto out;

error:
    ret.status = EP_FAILURE;
    ret.ep_read_block_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_read_block);
    return &ret;
}

ep_io_ret_t *ep_write_block_1_svc(ep_write_block_arg_t * arg,
        struct svc_req * req) {
    static ep_io_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    START_PROFILING_IO(ep_write_block, arg->length);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    if ((ret.ep_io_ret_t_u.length = export_write_block(exp, arg->fid, arg->bid, arg->nrb, arg->dist, arg->offset, arg->length)) < 0)
        goto error;
    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_io_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_write_block);
    return &ret;
}

/* not used anymore
ep_status_ret_t *ep_open_1_svc(ep_mfile_arg_t * arg, struct svc_req * req) {
    static ep_status_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_open(exp, arg->fid) != 0)
        goto error;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_status_ret_t_u.error = errno;
out:

    ret.status = EP_SUCCESS;
    return &ret;
}
 */
/*
ep_status_ret_t *ep_close_1_svc(ep_mfile_arg_t * arg, struct svc_req * req) {
    static ep_status_ret_t ret;

    export_t *exp;
    DEBUG_FUNCTION;

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_close(exp, arg->fid) != 0)
        goto error;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_status_ret_t_u.error = errno;
out:
    ret.status = EP_SUCCESS;
    return &ret;
}
 */

ep_status_ret_t *ep_setxattr_1_svc(ep_setxattr_arg_t * arg, struct svc_req * req) {
    static ep_status_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;
    
    START_PROFILING(ep_setxattr);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_setxattr(exp, arg->fid, arg->name, arg->value, arg->size, arg->flags) != 0) {
        goto error;
    }

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_status_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_setxattr);
    return &ret;
}

ep_getxattr_ret_t *ep_getxattr_1_svc(ep_getxattr_arg_t * arg, struct svc_req * req) {
    static ep_getxattr_ret_t ret;
    export_t *exp;
    ssize_t size = -1;
    DEBUG_FUNCTION;
    
    START_PROFILING(ep_getxattr);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    ret.ep_getxattr_ret_t_u.ret.value = xmalloc(ROZOFS_XATTR_VALUE_MAX);
    
    if ((size = export_getxattr(exp, arg->fid, arg->name, ret.ep_getxattr_ret_t_u.ret.value, arg->size)) == -1) {
        goto error;
    }

    ret.ep_getxattr_ret_t_u.ret.size = size;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_getxattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_getxattr);
    return &ret;
}

ep_status_ret_t *ep_removexattr_1_svc(ep_removexattr_arg_t * arg, struct svc_req * req) {
    static ep_status_ret_t ret;
    export_t *exp;
    DEBUG_FUNCTION;

    START_PROFILING(ep_removexattr);
    
    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;

    if (export_removexattr(exp, arg->fid, arg->name) != 0) {
        goto error;
    }

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_status_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_removexattr);
    return &ret;
}

ep_listxattr_ret_t *ep_listxattr_1_svc(ep_listxattr_arg_t * arg, struct svc_req * req) {
    static ep_listxattr_ret_t ret;
    export_t *exp;
    ssize_t size = -1;
    DEBUG_FUNCTION;
    
    START_PROFILING(ep_listxattr);

    if (!(exp = exports_lookup_export(arg->eid)))
        goto error;
    
    if ((size = export_listxattr(exp, arg->fid, ret.ep_listxattr_ret_t_u.ret.list, arg->size)) == -1) {
        goto error;
    }

    ret.ep_listxattr_ret_t_u.ret.size = size;

    ret.status = EP_SUCCESS;
    goto out;
error:
    ret.status = EP_FAILURE;
    ret.ep_listxattr_ret_t_u.error = errno;
out:
    STOP_PROFILING(ep_listxattr);
    return &ret;
}