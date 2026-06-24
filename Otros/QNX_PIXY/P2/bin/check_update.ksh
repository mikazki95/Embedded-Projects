#!/bin/sh

###########################
# Function to print trace #
# ----------------------- #
###########################

function print_trace 
{
	if test ! -z "$VERBOSE_"; then echo $1; fi
}

###########################################################
# Exporta la variable de entorno del directorio de update #
# ------------------------------------------------------- #
###########################################################
 
export TMP_FTP=/var/traintic/ftp_guest/pub

########################################################################
# Chequea si existen autoinstalables o ficheros a cargar               #
# -------------------------------------------------------------------- # 
########################################################################

if ! test "$NOT_EXEC_UPDATE"; then
   print_trace "Checking autoinstall packages or files to update .."
   NUM_FILES_INPUB=$( ls $TMP_FTP | wc -l )
   if test -f $TMP_FTP/remote_config_hmi.xml ; then 
     REMOTE_CONFIG_FILE_INPUB=1
   else
     REMOTE_CONFIG_FILE_INPUB=0
   fi
   NUM_UPDATE_FILES_INPUB=$( expr $NUM_FILES_INPUB - $REMOTE_CONFIG_FILE_INPUB - 2 )
   if test $NUM_UPDATE_FILES_INPUB -ge 1 ; then
     /home/common/scripts/mount_writable.ksh
     if test $( ls $TMP_FTP | grep mstt | wc -l ) -ge 1 ; then
       devb-ram disk name=ram ram capacity=0,nodinit blk ramdisk=150m,cache=0,vnode=256
       waitfor /dev/ram0 1
       dinit -q -h /dev/ram0
       mount -tqnx4 /dev/ram0 /ram
       #cd $TMP_FTP
       cd /ram 
       for f in $(ls -S $TMP_FTP)
       do
         if [[ $f = *.mstt ]]; then
           chmod 777 $TMP_FTP/$f
           $TMP_FTP/$f
           rm -f $TMP_FTP/$f
         fi
       done
       #umount /ram
       slay devb-ram
     fi
     
     if test -f /home/traintic/bin/UpdatableFiles.ksh ; then
       /home/traintic/bin/UpdatableFiles.ksh  
     fi
     
     /home/common/scripts/mount_readonly.ksh
     cd $TMP_FTP 
     rm -Rf !(remote_config_hmi.xml)
   fi
fi

