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

#include <limits.h>
#include <errno.h>

#include <rozofs/common/log.h>
#include <rozofs/common/xmalloc.h>
#include <rozofs/common/profile.h>
#include <rozofs/rpc/spproto.h>
#include <rozofs/rpc/mproto.h>

#include "storage.h"
#include "storaged.h"

DECLARE_PROFILING(spp_profiler_t);

void *mp_null_1_svc(void *args, struct svc_req *req) {
    DEBUG_FUNCTION;
    return 0;
}

mp_status_ret_t *mp_remove_1_svc(mp_remove_arg_t * args, struct svc_req * req) {
    static mp_status_ret_t ret;
    storage_t *st = 0;
    DEBUG_FUNCTION;

    ret.status = MP_FAILURE;
    if ((st = storaged_lookup(args->sid)) == 0) {
        ret.mp_status_ret_t_u.error = errno;
        goto out;
    }
    if (storage_rm_file(st, args->fid) != 0 && errno != ENOENT) {
        ret.mp_status_ret_t_u.error = errno;
        goto out;
    }

    ret.status = MP_SUCCESS;
out:
    return &ret;
}

mp_stat_ret_t *mp_stat_1_svc(uint16_t * sid, struct svc_req * req) {
    static mp_stat_ret_t ret;
    storage_t *st = 0;
    sstat_t sstat;
    DEBUG_FUNCTION;

    START_PROFILING(stat);

    ret.status = MP_FAILURE;
    if ((st = storaged_lookup(*sid)) == 0) {
        ret.mp_stat_ret_t_u.error = errno;
        goto out;
    }
    if (storage_stat(st, &sstat) != 0) {
        ret.mp_stat_ret_t_u.error = errno;
        goto out;
    }
    ret.mp_stat_ret_t_u.sstat.size = sstat.size;
    ret.mp_stat_ret_t_u.sstat.free = sstat.free;
    ret.status = MP_SUCCESS;
out:
    STOP_PROFILING(stat);
    return &ret;
}

mp_ports_ret_t *mp_ports_1_svc(void *args, struct svc_req * req) {
    static mp_ports_ret_t ret;

    DEBUG_FUNCTION;

    START_PROFILING(ports);
    ret.status = MP_FAILURE;

    memset(&ret.mp_ports_ret_t_u.ports, 0, STORAGE_NODE_PORTS_MAX * sizeof (uint32_t));

    if (!memcpy(&ret.mp_ports_ret_t_u.ports, storaged_storage_ports,
            storaged_nb_io_processes * sizeof (uint32_t))) {
        goto out;
    }

    ret.status = MP_SUCCESS;

out:
    STOP_PROFILING(ports);
    return &ret;
}
