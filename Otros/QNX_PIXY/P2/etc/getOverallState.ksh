#!/bin/sh
echo getOveralStatus eskripta exekutatzera doa
echo $CABINE_ID

#if [ "$CABINE_ID" = 1 ] ; then
#  OCTETO=$( ( sleep 4 ; echo admin ; sleep 2 ; echo admin ; sleep 4 ; echo " " ; sleep 1 ; echo "en" ; sleep 2 ; echo "sh ppp" ; sleep 4 ; echo "exit" ; sleep 2 ; echo "exit" ; slep 1 ) | telnet 172.20.154.1 2> /dev/null | grep ppp1 | cut -d . -f 4 )
#  OCTETO=$( ( sleep 4 ; echo root ; sleep 2 ; echo provis ; sleep 4 ; echo "/usr/bin/ifconfig en0" ; sleep 2 ; echo "exit" ; sleep 2 ) | telnet 172.20.154.251 2> /dev/null | grep inet | cut -c 18-20 )
#  echo "OCTETO=$OCTETO"
#   /usr/caf/utils/WriteDB IHM_M1_ipLastoctet "$OCTETO" f
#fi

while true
do
  if ! test "1" = $( ps | grep -E -c -eCdu ) ; then
    echo Cdu no esta en memoria. Ejecutar de nuevo proceso Cdu.
    /usr/caf/bin/Cdu CONFIG_FILE=/usr/caf/config/cduconfig.txt -ENABLE_CONSOLE
  fi
  sleep 4
done
