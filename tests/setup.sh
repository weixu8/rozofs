#!/bin/bash

#  Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
#  This file is part of Rozofs.
#  Rozofs is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published
#  by the Free Software Foundation, version 2.
#  Rozofs is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see
#  <http://www.gnu.org/licenses/>.

#
# setup.sh will generates a full working rozofs locally
# it's a useful tool for testing an debugging purposes. 
#

. env.sh

build ()
{
    if [ ! -e "${LOCAL_SOURCE_DIR}" ]
    then
        echo "Unable to build RozoFS (${LOCAL_SOURCE_DIR} not exist)"
    fi

    if [ -e "${LOCAL_BUILD_DIR}" ]
    then
        rm -rf ${LOCAL_BUILD_DIR}
    fi

    mkdir ${LOCAL_BUILD_DIR}

    cd ${LOCAL_BUILD_DIR}
    rm -f ${LOCAL_SOURCE_DIR}/CMakeCache.txt
    cmake -G "Unix Makefiles" -DDAEMON_PID_DIRECTORY=${BUILD_DIR} -DCMAKE_BUILD_TYPE=${LOCAL_CMAKE_BUILD_TYPE} ${LOCAL_SOURCE_DIR}
    make
    cd ..
    cp -r ${LOCAL_SOURCE_DIR}/tests/fs_ops/pjd-fstest/tests ${LOCAL_PJDTESTS}
}

# $1 -> LAYOUT
# $2 -> storages by node
gen_storage_conf ()
{
    ROZOFS_LAYOUT=$1
    STORAGES_BY_CLUSTER=$2

    FILE=${LOCAL_CONF}'storage_l'${ROZOFS_LAYOUT}'.conf'

    if [ ! -e "$LOCAL_CONF" ]
    then
        mkdir -p $LOCAL_CONF
    fi

    if [ -e "$FILE" ]
    then
        rm -rf $FILE
    fi

    touch $FILE
    echo "#${NAME_LABEL}" >> $FILE
    echo "#${DATE_LABEL}" >> $FILE
    echo "layout = ${ROZOFS_LAYOUT} ;" >> $FILE
    echo "ports = [ 40000, 40001, 40002, 40003 ] ;" >> $FILE
    echo 'storages = (' >> $FILE
    let nb_storages=$((${STORAGES_BY_CLUSTER}*${NB_CLUSTERS_BY_VOLUME}*${NB_VOLUMES}))
    for j in $(seq ${nb_storages}); do
        if [[ ${j} == ${nb_storages} ]]
        then
            echo "  {sid = $j; root =\"${LOCAL_STORAGES_ROOT}_$j\";}" >> $FILE
        else
            echo "  {sid = $j; root =\"${LOCAL_STORAGES_ROOT}_$j\";}," >> $FILE
        fi
    done;
    echo ');' >> $FILE
}

# $1 -> LAYOUT
# $2 -> storages by node
# $2 -> Nb. of exports
# $3 -> md5 generated
gen_export_conf ()
{

    ROZOFS_LAYOUT=$1

    FILE=${LOCAL_CONF}'export_l'${ROZOFS_LAYOUT}'.conf'

    if [ ! -e "$LOCAL_CONF" ]
    then
        mkdir -p $LOCAL_CONF
    fi

    if [ -e "$FILE" ]
    then
        rm -rf $FILE
    fi

    touch $FILE
    echo "#${NAME_LABEL}" >> $FILE
    echo "#${DATE_LABEL}" >> $FILE
    echo "layout = ${ROZOFS_LAYOUT} ;" >> $FILE
    echo 'volumes =' >> $FILE
    echo '      (' >> $FILE

        for v in $(seq ${NB_VOLUMES}); do
            echo '        {' >> $FILE
            echo "            vid = $v;" >> $FILE
            echo '            cids= ' >> $FILE
            echo '            (' >> $FILE

            for c in $(seq ${NB_CLUSTERS_BY_VOLUME}); do
            let idx_cluster=(${v}-1)*${NB_CLUSTERS_BY_VOLUME}+${c}
            echo '                   {' >> $FILE
            echo "                       cid = $idx_cluster;" >> $FILE
            echo '                       sids =' >> $FILE
            echo '                       (' >> $FILE
                for k in $(seq ${STORAGES_BY_CLUSTER}); do
                    let idx=${k}-1;
                     idx_tmp_1=$(((${v}-1)*${NB_CLUSTERS_BY_VOLUME}*${STORAGES_BY_CLUSTER}))
                     idx_tmp_2=$((${STORAGES_BY_CLUSTER}*(${c}-1)))
                     idx_storage=$((${idx_tmp_1}+${idx_tmp_2}+${k}))
                    if [[ ${k} == ${STORAGES_BY_CLUSTER} ]]
                    then
                        echo "                           {sid = ${idx_storage}; host = \"${LOCAL_STORAGE_NAME_BASE}\";}" >> $FILE
                    else
                        echo "                           {sid = ${idx_storage}; host = \"${LOCAL_STORAGE_NAME_BASE}\";}," >> $FILE
                    fi
                done;
                echo '                       );' >> $FILE
                if [[ ${c} == ${NB_CLUSTERS_BY_VOLUME} ]]
                then
                    echo '                   }' >> $FILE
                else
                    echo '                   },' >> $FILE
                fi
            done;
        echo '            );' >> $FILE
        if [[ ${v} == ${NB_VOLUMES} ]]
        then
            echo '        }' >> $FILE
        else
            echo '        },' >> $FILE
        fi
        done;
    echo '    )' >> $FILE
    echo ';' >> $FILE

    echo 'exports = (' >> $FILE
    for k in $(seq ${NB_EXPORTS}); do
        if [[ ${k} == ${NB_EXPORTS} ]]
        then
            echo "   {eid = $k; root = \"${LOCAL_EXPORTS_ROOT}_$k\"; md5=\"${3}\"; squota=\"\"; hquota=\"\"; vid=${k};}" >> $FILE
        else
            echo "   {eid = $k; root = \"${LOCAL_EXPORTS_ROOT}_$k\"; md5=\"${3}\"; squota=\"\"; hquota=\"\"; vid=${k};}," >> $FILE
        fi
    done;
    echo ');' >> $FILE
}

start_storaged ()
{

    echo "------------------------------------------------------"
    PID=`ps ax | grep ${LOCAL_STORAGE_DAEMON} | grep -v grep | awk '{print $1}'`
    if [ "$PID" == "" ]
    then
        echo "Start ${LOCAL_STORAGE_DAEMON}"
    	${LOCAL_BINARY_DIR}/storaged/${LOCAL_STORAGE_DAEMON} -c ${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE}
    else
        echo "Unable to start ${LOCAL_STORAGE_DAEMON} (already running as PID: ${PID})"
        exit 0;
    fi

}

stop_storaged ()
{
    echo "------------------------------------------------------"
    PID=`ps ax | grep ${LOCAL_STORAGE_DAEMON} | grep -v grep | awk '{print $1}'`
    if [ "$PID" != "" ]
    then
        echo "Stop ${LOCAL_STORAGE_DAEMON} (PID: ${PID})"
        kill $PID
    else
        echo "Unable to stop ${LOCAL_STORAGE_DAEMON} (not running)"
    fi
}

reload_storaged ()
{
    echo "------------------------------------------------------"
    echo "Reload ${LOCAL_STORAGE_DAEMON}"
    kill -1 `ps ax | grep ${LOCAL_STORAGE_DAEMON} | grep -v grep | awk '{print $1}'`
}

# $1 -> storages by node
create_storages ()
{

    if [ ! -e "${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE}" ]
    then
        echo "Unable to remove storage directories (configuration file doesn't exist)"
    else
        STORAGES_BY_CLUSTER=`grep sid ${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE} | wc -l`

        for j in $(seq ${STORAGES_BY_CLUSTER}); do

            if [ -e "${LOCAL_STORAGES_ROOT}_${j}" ]
            then
                rm -rf ${LOCAL_STORAGES_ROOT}_${j}/*.bins
            else
                mkdir -p ${LOCAL_STORAGES_ROOT}_${j}
            fi

        done;
    fi
}

# $1 -> storages by node
remove_storages ()
{
    if [ ! -e "${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE}" ]
    then
        echo "Unable to remove storage directories (configuration file doesn't exist)"
    else
        STORAGES_BY_CLUSTER=`grep sid ${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE} | wc -l`

        for j in $(seq ${STORAGES_BY_CLUSTER}); do

            if [ -e "${LOCAL_STORAGES_ROOT}_${j}" ]
            then
                rm -rf ${LOCAL_STORAGES_ROOT}_${j}
            fi

        done;
    fi
}

# $1 -> LAYOUT
go_layout ()
{
    ROZOFS_LAYOUT=$1

    if [ ! -e "${LOCAL_CONF}export_l${ROZOFS_LAYOUT}.conf" ] || [ ! -e "${LOCAL_CONF}export_l${ROZOFS_LAYOUT}.conf" ]
    then
        echo "Unable to change configuration files to layout ${ROZOFS_LAYOUT}"
        exit 0
    else
        ln -s -f ${LOCAL_CONF}'export_l'${ROZOFS_LAYOUT}'.conf' ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}
        ln -s -f ${LOCAL_CONF}'storage_l'${ROZOFS_LAYOUT}'.conf' ${LOCAL_CONF}${LOCAL_STORAGE_CONF_FILE}
    fi
}

deploy_clients_local ()
{
    echo "------------------------------------------------------"
    if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
        then
        echo "Unable to mount RozoFS (configuration file doesn't exist)"
    else
        NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`

        for j in $(seq ${NB_EXPORTS}); do
            mountpoint -q ${LOCAL_MNT_ROOT}${j}
            if [ "$?" -ne 0 ]
            then
                echo "Mount RozoFS (export: ${LOCAL_EXPORTS_NAME_PREFIX}_${j}) on ${LOCAL_MNT_PREFIX}${j}"

                if [ ! -e "${LOCAL_MNT_ROOT}${j}" ]
                then
                    mkdir -p ${LOCAL_MNT_ROOT}${j}
                fi

                ${LOCAL_BINARY_DIR}/rozofsmount/${LOCAL_ROZOFS_CLIENT} -H ${LOCAL_EXPORT_NAME_BASE} -E ${LOCAL_EXPORTS_ROOT}_${j} ${LOCAL_MNT_ROOT}${j}
            else
                echo "Unable to mount RozoFS (${LOCAL_MNT_PREFIX}_${j} already mounted)"
            fi
        done;
    fi
}

undeploy_clients_local ()
{
    echo "------------------------------------------------------"
    if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
        then
        echo "Unable to umount RozoFS (configuration file doesn't exist)"
    else
        NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`

        for j in $(seq ${NB_EXPORTS}); do
            echo "Umount RozoFS mnt: ${LOCAL_MNT_PREFIX}${j}"
            umount ${LOCAL_MNT_ROOT}${j}
            test -d ${LOCAL_MNT_ROOT}${j} && rm -rf ${LOCAL_MNT_ROOT}${j}
        done;
    fi
}

start_exportd ()
{
    echo "------------------------------------------------------"
    PID=`ps ax | grep ${LOCAL_EXPORT_DAEMON} | grep -v grep | awk '{print $1}'`
    if [ "$PID" == "" ]
    then
        echo "Start ${LOCAL_EXPORT_DAEMON}"
        ${LOCAL_BINARY_DIR}/exportd/${LOCAL_EXPORT_DAEMON} -c ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}
    else
        echo "Unable to start ${EXPORT_DAEMON} (already running as PID: ${PID})"
        exit 0;
    fi
}

stop_exportd ()
{
    echo "------------------------------------------------------"
    PID=`ps ax | grep ${LOCAL_EXPORT_DAEMON} | grep -v grep | awk '{print $1}'`
    if [ "$PID" != "" ]
    then
        echo "Stop ${LOCAL_EXPORT_DAEMON} (PID: ${PID})"
        kill $PID
    else
        echo "Unable to stop ${LOCAL_EXPORT_DAEMON} (not running)"
    fi
}

reload_exportd ()
{
    echo "------------------------------------------------------"
    PID=`ps ax | grep ${LOCAL_EXPORT_DAEMON} | grep -v grep | awk '{print $1}'`
    if [ "$PID" != "" ]
    then
        echo "Reload ${LOCAL_EXPORT_DAEMON} (PID: ${PID})"
        kill -1 $PID
    else
        echo "Unable to reload ${LOCAL_EXPORT_DAEMON} (not running)"
    fi
}

# $1 -> Nb. of exports
create_exports ()
{
    if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
    then
        echo "Unable to create export directories (configuration file doesn't exist)"
    else
        NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`

        for k in $(seq ${NB_EXPORTS}); do
            if [ -e "${LOCAL_EXPORTS_ROOT}_${k}" ]
            then
                rm -rf ${LOCAL_EXPORTS_ROOT}_${k}/*
            else
                mkdir -p ${LOCAL_EXPORTS_ROOT}_${k}
            fi
        done;
    fi
}

# $1 -> Nb. of exports
remove_exports ()
{
    if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
    then
        echo "Unable to remove export directories (configuration file doesn't exist)"
    else
        NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`

        for j in $(seq ${NB_EXPORTS}); do

            if [ -e "${LOCAL_EXPORTS_ROOT}_${j}" ]
            then
                rm -rf ${LOCAL_EXPORTS_ROOT}_${j}
            fi
        done;
    fi
}

remove_config_files ()
{
    echo "------------------------------------------------------"
    echo "Remove configuration files"
    rm -rf $LOCAL_CONF
}

remove_all ()
{
    echo "------------------------------------------------------"
    echo "Remove configuration files, storage and exports directories"
    rm -rf $LOCAL_CONF
    rm -rf $LOCAL_STORAGES_ROOT*
    rm -rf $LOCAL_EXPORTS_ROOT*
}

remove_build ()
{
    echo "------------------------------------------------------"
    echo "Remove build directory"
    rm -rf $LOCAL_BUILD_DIR
}

clean_all ()
{
    undeploy_clients_local
    stop_storaged
    stop_exportd
    remove_build
    remove_all
}

check_no_run ()
{

    PID_EXPORTD=`ps ax | grep ${LOCAL_EXPORT_DAEMON} | grep -v "grep" | awk '{print $1}'`
    PID_STORAGED=`ps ax | grep ${LOCAL_STORAGE_DAEMON} | grep -v "grep" | awk '{print $1}'`

    if [ "$PID_STORAGED" != "" ] || [ "$PID_EXPORTD" != "" ]
    then
        echo "${LOCAL_EXPORT_DAEMON} or/and ${LOCAL_STORAGE_DAEMON} already running"
        exit 0;
    fi

}

check_build ()
{

    if [ ! -e "${LOCAL_BINARY_DIR}/exportd/${LOCAL_EXPORT_DAEMON}" ]
    then
        echo "Daemons are not build !!! use $0 build"
        exit 0;
    fi

}

pjd_test()
{

    if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
        then
        echo "Unable to run pjd tests (configuration file doesn't exist)"
    else
		NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`
		EXPORT_LAYOUT=`grep layout ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | grep -v grep | cut -c 10`

		for j in $(seq ${NB_EXPORTS}); do
		    echo "------------------------------------------------------"
		    mountpoint -q ${LOCAL_MNT_ROOT}${j}
		    if [ "$?" -eq 0 ]
		    then
		        echo "Run pjd tests on ${LOCAL_MNT_PREFIX}${j} with layout $EXPORT_LAYOUT"
		        echo "------------------------------------------------------"

		        cd ${LOCAL_MNT_ROOT}${j}
		        prove -r ${LOCAL_PJDTESTS}
		        cd ..

		    else
		        echo "Unable to run pjd tests (${LOCAL_MNT_PREFIX}${j} is not mounted)"
		    fi
		done;
	fi
}

fileop_test(){

	if [ ! -e "${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE}" ]
		    then
		    echo "Unable to run pjd tests (configuration file doesn't exist)"
		else
			LOWER_LMT=1
			UPPER_LMT=4
			INCREMENT=1
			FILE_SIZE=2M

			NB_EXPORTS=`grep eid ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | wc -l`
			EXPORT_LAYOUT=`grep layout ${LOCAL_CONF}${LOCAL_EXPORT_CONF_FILE} | grep -v grep | cut -c 10`

			for j in $(seq ${NB_EXPORTS}); do
				echo "------------------------------------------------------"
				mountpoint -q ${LOCAL_MNT_ROOT}${j}
				if [ "$?" -eq 0 ]
				then
				    echo "Run fileop test on ${LOCAL_MNT_PREFIX}${j} with layout $EXPORT_LAYOUT"
				    echo "------------------------------------------------------"
				    ${FSOP_BINARY} -l ${LOWER_LMT} -u ${UPPER_LMT} -i ${INCREMENT} -e -s ${FILE_SIZE} -d ${LOCAL_MNT_ROOT}${j}
				else
				    echo "Unable to run fileop test (${LOCAL_MNT_PREFIX}${j} is not mounted)"
				fi
			done;
	fi
}


usage ()
{
    echo >&2 "Usage:"
    echo >&2 "$0 start <Layout>"
    echo >&2 "$0 stop"
    echo >&2 "$0 reload"
    echo >&2 "$0 build"
    echo >&2 "$0 clean"
    echo >&2 "$0 pjd_test"
    echo >&2 "$0 fileop_test"
    echo >&2 "$0 mount"
    echo >&2 "$0 umount"
    exit 0;
}

main ()
{
    [ $# -lt 1 ] && usage

    if [ "$1" == "start" ]
    then

        [ $# -lt 2 ] && usage

        if [ "$2" -eq 0 ]
        then
            ROZOFS_LAYOUT=$2
            STORAGES_BY_CLUSTER=4
        elif [ "$2" -eq 1 ]
        then
            ROZOFS_LAYOUT=$2
            STORAGES_BY_CLUSTER=8
        elif [ "$2" -eq 2 ]
        then
            ROZOFS_LAYOUT=$2
            STORAGES_BY_CLUSTER=16
        else
	        echo >&2 "Rozofs layout must be equal to 0,1 or 2."
	        exit 1
        fi

        check_build
        check_no_run

        NB_EXPORTS=2
        NB_VOLUMES=2;
        NB_CLUSTERS_BY_VOLUME=2;

        gen_storage_conf ${ROZOFS_LAYOUT} ${STORAGES_BY_CLUSTER}
        gen_export_conf ${ROZOFS_LAYOUT} ${STORAGES_BY_CLUSTER}

        go_layout ${ROZOFS_LAYOUT}

        create_storages
        create_exports

        start_storaged
        start_exportd

        deploy_clients_local

    elif [ "$1" == "stop" ]
    then

        undeploy_clients_local

        stop_storaged
        stop_exportd

        remove_all
    elif [ "$1" == "reload" ]
    then

        undeploy_clients_local

        reload_storaged
        reload_exportd

        deploy_clients_local

    elif [ "$1" == "pjd_test" ]
    then
        check_build
        pjd_test
    elif [ "$1" == "fileop_test" ]
    then
        check_build
        fileop_test

    elif [ "$1" == "mount" ]
    then
        check_build
        deploy_clients_local

    elif [ "$1" == "umount" ]
    then
        check_build
        undeploy_clients_local

    elif [ "$1" == "build" ]
    then
        build
    elif [ "$1" == "clean" ]
    then
        clean_all
    else
        usage
    fi
    exit 0;
}

main $@
