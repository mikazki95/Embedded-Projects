#/bin/sh

# Funcion que mueve un fichero a su ruta correcta y le asigna el usuario grupo y permisos 
# adecuados en caso de que sea cargable y este en el pub del ftp.
testPubAndMove()
{
  if test -f $TMP_FTP/$FNAME ; then
    if [ $( echo $FATTR | grep U ) ] ; then
      mv $TMP_FTP/$FNAME $FPATH/$FNAME
      chown root:root $FPATH/$FNAME
      chmod $FPERM $FPATH/$FNAME
    fi
  fi
}

# Funcion que mueve un fichero a su ruta correcta y le asigna el usuario grupo y permisos 
# adecuados en caso de que sea cargable y este en el pub del ftp. 
# Ademas elimina los ficheros dependientes de la configuracion de alarmas
testPubAndMoveAndRemoveAlarmsDependant()
{
  if test -f $TMP_FTP/$FNAME ; then
    if [ $( echo $FATTR | grep U ) ] ; then
      testPubAndMove
      rm -f /var/traintic/hist/AlarmReg*
      rm -f /var/traintic/hist/CounterReg
      rm -f /var/traintic/hist/LocalReg
    fi
  fi
}

# Funcion que mueve un fichero a su ruta correcta y le asigna el usuario grupo y permisos 
# adecuados en caso de que sea cargable y este en el pub del ftp. 
# Ademas elimina los ficheros dependientes de la configuracion del bus MVB
testPubAndMoveAndRemoveMVBBusConfigDependant()
{
  if test -f $TMP_FTP/$FNAME ; then
    if [ $( echo $FATTR | grep U ) ] ; then
      testPubAndMove
      rm -f /var/traintic/hist/MVBPerVarDB.bin
    fi
  fi
}

# MESSAGES_LIST
MESG1_CODE="trmsgUploadRemoveWarning";
MESG1_DATA="AlarmReg00, AlarmReg01, hmi1.nsdb, hmi2.nsdb";
MESG2_CODE="trmsgUploadRemoveWarning";
MESG2_DATA="MVBPerVarDB.bin";

# FILES_DESCRIPTION 

# Ficheros cargables
FNAME=hmi1.nsdb;                FPATH=/home/traintic/config;  FPERM=774; FTYPE=binary; FATTR=DU; MESGC=MESG2_CODE; MESGD=MESG2_DATA; testPubAndMoveAndRemoveMVBBusConfigDependant
FNAME=hmi2.nsdb;                FPATH=/home/traintic/config;  FPERM=774; FTYPE=binary; FATTR=DU; MESGC=MESG2_CODE; MESGD=MESG2_DATA; testPubAndMoveAndRemoveMVBBusConfigDependant
FNAME=ethpd.nsdb;               FPATH=/home/traintic/config;  FPERM=774; FTYPE=binary; FATTR=DU; MESGC=MESG2_CODE; MESGD=MESG2_DATA; testPubAndMoveAndRemoveMVBBusConfigDependant
FNAME=HMIAlarms.xml;            FPATH=/home/traintic/HMIData; FPERM=774; FTYPE=text;   FATTR=DU; MESGC=MESG1_CODE; MESGD=MESG1_DATA; testPubAndMoveAndRemoveAlarmsDependant
FNAME=marshallVars_C1.csv;      FPATH=/home/traintic/config;  FPERM=774; FTYPE=text;   FATTR=DU; testPubAndMove
FNAME=marshallVars_C2.csv;      FPATH=/home/traintic/config;  FPERM=774; FTYPE=text;   FATTR=DU; testPubAndMove
FNAME=DeviceDescriptor.cfg;     FPATH=/home/traintic/HMIData; FPERM=774; FTYPE=text;   FATTR=DU; testPubAndMove
FNAME=md5.pwd;                  FPATH=/home/traintic/HMIData; FPERM=774; FTYPE=binary; FATTR=DU; testPubAndMove
FNAME=VersionDescriptorQNX.cfg; FPATH=/home/traintic/HMIData; FPERM=774; FTYPE=text;   FATTR=DU; testPubAndMove

# Ficheros descargables adicionales
FNAME=AlarmReg00;           FPATH=/var/traintic/hist;                FTYPE=binary; FATTR=D; 

# Otros ficheros de proyecto. Se ignoran.
#FNAME=cduconfig.txt;       FPATH=/home/traintic/config
#FNAME=HMIInit.ini;         FPATH=/home/traintic/config
#FNAME=HMIInitD.ini;        FPATH=/home/traintic/config
#FNAME=HMIKeypad.cfg;       FPATH=/home/traintic/config
#FNAME=HMIBoot.ksh;         FPATH=/home/traintic/bin/
#FNAME=caf_start.ksh;       FPATH=/home/traintic/bin/
#FNAME=CabineConfig.ksh;    FPATH=/home/traintic/etc/
#FNAME=SuperviseMainIP.ksh; FPATH=/home/traintic/etc/
#FNAME=HMIBoot.bot;         FPATH=/var/traintic/config/
