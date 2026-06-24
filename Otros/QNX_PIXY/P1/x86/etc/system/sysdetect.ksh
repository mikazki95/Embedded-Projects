#!/bin/ksh

pidin info | grep "686 IDT 686 F6M13S0 1000MHz FPU"
if [[ $? -eq 0 ]] then
 pci -vv | grep "1969h,  ES1969 Solo-1 Audiodrive" 
 if [[ $? -eq 0 ]] then  
  echo "Gersys BC3712 detected"
  echo "Gersys_BC3712" > /tmp/target
  export HW=Gersys_BC3712
 else
  echo "Gersys BC5711 detected"
  echo "Gersys_BC5711" > /tmp/target
  export HW=Gersys_BC5711
 fi
else
 pidin info | grep "686 IDT 686 F6M15S10"
 if [[ $? -eq 0 ]] then
  echo "Gersys BC3811 detected"
  echo "Gersys_BC3811" > /tmp/target
  export HW=Gersys_BC3811
 else
  pidin info | grep "686 IDT 686 F6M13S0 501MHz FPU"
  if [[ $? -eq 0 ]] then
   echo "Gersys BC3510 detected"
   echo "Gersys_BC3510" > /tmp/target
   export HW=Gersys_BC3510
  else
   pci -vv | grep "27d0h,  82801G (ICH7 Family) PCI Express" 
   if [[ $? -eq 0 ]] then
    echo "Pixy_INC_80 detected" 
    echo "Pixy_INC_80" > /tmp/target
    export HW=Pixy_INC_80
   else
    pci -vv | grep "Display (Other)"
    if [[ $? -eq 0 ]] then
     echo "Pixy_INC_70_kontron detected"
     echo "Pixy_INC_70_kontron" > /tmp/target
     export HW=Pixy_INC_70_kontron
    else
     echo "Pixy_INC_70_congatec detected" 
     echo "Pixy_INC_70_congatec" > /tmp/target
     export HW=Pixy_INC_70_congatec
    fi
   fi
  fi
 fi
fi
