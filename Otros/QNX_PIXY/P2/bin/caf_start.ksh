#!/bin/sh

echo "starting caf_start"

while getopts svbe  opt $*
do
	case $opt in
		s)	export NOT_EXEC_UPDATE=1;;
		e)	export VERBOSE_=1;;
		b)	export NOT_EXEC_HMIBOOT=1;;
		v)	set -x;;
		*)	echo; echo
			use $0
			exit 1;;
	esac
done

typeset -i VERBOSE SAFEMODE NO_ETC NOT_EXEC_UPDATE NOT_EXEC_HMIBOOT VERBOSE_
typeset +x VERBOSE SAFEMODE NO_ETC NOT_EXEC_UPDATE NOT_EXEC_HMIBOOT VERBOSE_


# Function to print trace
# -----------------------
function print_trace
{
	if test ! -z "$VERBOSE_"; then echo $1; fi
}

# Saca traza de comienzo
# ----------------------
print_trace "Starting CAF-HMI V.1.0"
chkfsys -P /dev/hd0t77
# Exporta la variable de entorno del directorio de update
# -------------------------------------------------------
export TMP_FTP=/var/traintic/ftp_guest/pub


ifconfig en0 172.20.154.251 netmask 255.255.255.0
route add default 172.20.154.1

inetd &

if test ! -e /home/traintic/etc/dwnld_start;then
  print_trace "Download starts ..."
  exec /home/traintic/etc/dwnld_start
fi


# Exporta variables de entorno
# ----------------------------
#La TZ en Mexico es: export TZ=CST+6,M4.1.0/02:00,M10.5.0/02:00
#Se elimina el cambio horario de invierno/verano:
#export TZ=CST+6

export TZ=CST+6
typeset -x TZ

if [ $HW = "Gersys_BC3712" ]
then
   export MMEX_BIN=/home/traintic/bin/MMEX_Gersys
elif [ $HW = "Pixy_INC_70_kontron" ]
then
   export MMEX_BIN=/home/traintic/bin/MMEX_Pixy
else
   echo "################################################################################"
   echo "HMIBoot.ksh: HW no valido. Compatible con Gersys_BC3712 y Pixy_INC_70_kontron"
   echo "################################################################################"
fi

# Inicializacion de la placa de entorno
# Se chequea si HMIBoot ya est� ejecutandose, en cuyo caso no se vuelve a lanzar
if test ! "$NOT_EXEC_HMIBOOT"; then
  # Ejecuta el comando de HMIBoot
  if test -f $INITIALIZER_HMIBOOT; then
	 print_trace "HMIBoot Starts ..."
	 exec $INITIALIZER_HMIBOOT WDOG=0 FR=0.5 BOOT_FILE=/var/traintic/config/HMIBoot.bot BUZZER_ENABLE=2 DISPLAY=1 INP_PIPE=boot BRIGHTNESS=200 BR_OFFSET=80 CTRL_TEMP_MIN=0 CTRL_TEMP_MAX=63 KEYPAD_FILE=$TTICCONFIG/HMIKeypad.cfg KEYPAD_MAP_FILE=$TTICCONFIG/HMIMap.cfg KEYPAD_FUNC_FILE=$TTICCONFIG/HMIFunc.cfg &
  fi
fi
