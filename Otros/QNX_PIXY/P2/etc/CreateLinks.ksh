#!/bin/sh

if ! test -f /home/traintic/HMIData/AlarmReg00; then
	if test -f /var/traintic/hist/AlarmReg00; then	
		/home/common/scripts/mount_writable.ksh
		ln -s /var/traintic/hist/AlarmReg00 /home/traintic/HMIData/AlarmReg00
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/prog.cfg; then
	if test -f /var/traintic/config/prog.cfg; then	
		/home/common/scripts/mount_writable.ksh
		ln -s /var/traintic/config/prog.cfg /home/traintic/HMIData/prog.cfg
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/ETHPDDescriptor.cfg; then
	if test -f /var/traintic/hist/ETHPDDescriptor.cfg; then	
		/home/common/scripts/mount_writable.ksh
		ln -s /var/traintic/hist/ETHPDDescriptor.cfg /home/traintic/HMIData/ETHPDDescriptor.cfg
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/HMIAlarms.xml; then
	if test -f /home/traintic/config/HMIAlarms.xml; then	
		/home/common/scripts/mount_writable.ksh
		ln -s /var/traintic/hist/HMIAlarms.xml /home/traintic/HMIData/HMIAlarms.xml
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/MVBDescriptor.cfg; then
	if test -f /var/traintic/hist/MVBDescriptor.cfg; then	
		/home/common/scripts/mount_writable.ksh
		ln -s /var/traintic/hist/MVBDescriptor.cfg /home/traintic/HMIData/MVBDescriptor.cfg
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/dwnld_start; then
	if test -f /home/traintic/etc/dwnld_start; then	
		/home/common/scripts/mount_writable.ksh
		ln -s  /home/traintic/etc/dwnld_start /home/traintic/HMIData/dwnld_start
		/home/common/scripts/mount_readonly.ksh
	fi
fi

if ! test -f /home/traintic/HMIData/dwnld_stop; then
	if test -f  /home/traintic/etc/dwnld_stop; then	
		/home/common/scripts/mount_writable.ksh
		ln -s  /home/traintic/etc/dwnld_stop /home/traintic/HMIData/dwnld_stop
		/home/common/scripts/mount_readonly.ksh
	fi
fi