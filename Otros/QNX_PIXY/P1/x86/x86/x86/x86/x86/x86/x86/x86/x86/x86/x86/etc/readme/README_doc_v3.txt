
 ********************************************************************************
 *                                                                              *
 *  DiskOnChip(R) driver for QNX-6.x (x86 processors).                          *
 *                                                                              *
 *  Version 6.2.2.                                                              *
 *                                                                              *
 *  INSTALLATION GUIDE.                                                         *
 *                                                                              *
 *  Copyright M-Systems (c) 2004                                                *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *   Use of this software with devices other then DiskOnChip is prohibited.     *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *                            I M P O R T A N T                                 *
 *                                                                              *
 *  This version of DiskOnChip driver works ONLY with the following members of  *
 *  M-Systems' DiskOnChip product line:                                         *
 *                                                                              *
 *      MD24xx                                                                  *
 *      MD43xx                                                                  *
 *      MD48xx                                                                  *
 *      MD58xx                                                                  *
 *                                                                              *
 *  If you are using DiskOnChip parts of different types, for example any of    *
 *  the following:                                                              *
 *                                                                              *
 *      MD22xx                                                                  *
 *      MD28xx                                                                  *
 *      MD33xx                                                                  *
 *      MD38xx                                                                  *
 *                                                                              *
 *  this version of DiskOnChip driver will *NOT* work with these parts. You     *
 *  must use DiskOnChip driver version 5.x.x with all such parts. You can       *
 *  download latest 5.x.x version (5.1.4 at the time of writing) of DiskOnChip  *
 *  driver for QNX-6 from M-Systems web site http://www.m-sys.com.              *
 *                                                                              *
 ********************************************************************************
    

VERSION MARK
------------

  Version 6.2.2    September 01, 2004
  Version 6.2.0    November 27, 2003
  Version 6.1.0    September 02, 2003


GENERAL
--------

  This directory contains QNX 6.x device driver and utilities for the following
  members of DiskOnChip product line:

      MD24xx     
      MD43xx     
      MD48xx     
      MD58xx     

  If you are using DiskOnChip parts of different types, for example any of the
  following:

      MD22xx     
      MD28xx     
      MD33xx     
      MD38xx     

  This version of DiskOnChip driver will NOT work with these parts. You must
  use DiskOnChip driver version 5.x.x with all such parts. You can download
  the latest 5.x.x version (5.1.4 at the time of writing) of DiskOnChip driver
  for QNX 6.x from M-Systems web site http://www.m-sys.com.

  In the remainder of this Readme, term "DiskOnChip" is used to refer to the
  functionality which is common to all types of DiskOnChips that are supported
  by this version of DiskOnChip driver.

  Some referrals are made from this document to the DiskOnChip DOS utilities.
  For detailed information on how to use these utilities, refer to "DiskOnChip
  Utilities User Manual". Updated manuals and documentation, can be found on 
  M-Systems web site (http://www.m-sys.com). For more information, contact 
  M-Systems via e-mail to info@m-sys.com, or by calling your local M-Systems
  office (see Chapter 12 below).


CONTENTS
--------

    1. Introduction
    2. Installing DiskOnChip driver
    3. Installing QNX 6.x to the DiskOnChip
    4. Configuring driver for various types of hardware interfaces to DiskOnChip
    5. Driver's configuration options
    6. Formatting DiskOnChip
    7. Access to DiskOnChip's Extended Functions
    8. Considerations related to unexpected power shutdowns
    9. Considerations related to I/O determinism
   10. Known limitations
   11. Troubleshooting
   12. Contact information
    Appendix A. Example of access to DiskOnChip's Extended Functions
    Appendix B. Booting DOS from DiskOnChip


1. INTRODUCTION
---------------

1.1. When used under QNX 6.x, DiskOnChip is managed by the dedicated device
     driver 'devb-doc3'. DiskOnChip driver is normally used with QNX file 
     system.

1.2. DiskOnChip driver uses term "socket" to refer to either of the following:

         - single DiskOnChip MD24xx part
         - single DiskOnChip MD4331 part, or up to 4 such cascaded parts
         - single DiskOnChip MD4811 part, or up to 4 such cascaded parts
         - single DiskOnChip MD4832 part, or up to 4 such cascaded parts
         - single DiskOnChip MD5811 part, or up to 4 such cascaded parts
         - single DiskOnChip MD5832 part, or up to 4 such cascaded parts

     The term 'cascaded' refers to the parts which all share the same address
     in the host's address space, and form a single continuous flash media of
     capacity equal to the sum of capacities of individual cascaded parts.

     DiskOnChip driver supports up to 4 "sockets", all possibly of different
     types / cascade configurations / capacities.

1.3. The flash media that constitutes DiskOnChip "socket", can be optionally
     divided into up to four independent regions (refered to as "disks"). All 
     such regions ("disks") can be read/write accessed independently from the 
     other "disks" of that "socket", and can have it's own hardware-level 
     read/write access permissions.

1.4. Each "disk" can be partitioned (with QNX 'fdisk' utility) into up to four
     disk partitions. Each such disk partition could be individually formatted 
     with QNX 'dinit' utility, and mounted with QNX 'mount' utility.


2. INSTALLING DISKONCHIP DRIVER
-------------------------------

2.1. This version of DiskOnChip driver works with the following members of 
     DiskOnChip product line ONLY:

         MD24xx
         MD4331
         MD4811
         MD4832
         MD5811
         MD5832

     If you are using DiskOnChip parts of different types, for example any of
     the following:

         MD22xx
         MD28xx
         MD33xx
         MD38xx

     this version of iskOnChip driver will *NOT* work with these parts. You
     must use DiskOnChip driver version 5.x.x with all such parts. You can 
     download latest 5.x.x version (5.1.4 at the time of writing) of DiskOnChip
     driver for QNX 6.x from M-Systems' web site http://www.m-sys.com.

2.2. Distribution of this version of DiskOnChip driver for QNX 6.x/x86 consists 
     of the archive diskonchip622-qnx6-x86.tar.gz, which contains the 
     following files:

         - README.TXT              This file

         - devb-doc3               DiskOnChip driver for QNX 6.x/x86

         - dformat3                DiskOnChip formatter for QNX 6.x/x86
 
         - bios.diskonchip.build   Sample "build" file for creating QNX 6.x/x86
                                   bootimages for booting from DiskOnChip

         - include                 Directory containing DiskOnChip header files. 
                                   These header files must be #included by the  
                                   application in case when it accesses  
                                   DiskOnChip's advanced features (see Chapter 7)

2.3. QNX Momentics 6.x could be installed on hosts running Windows, Solaris
     or QNX 6.x operating systems.

     If you have installed QNX Momentics 6.x on QNX 6.2.x host, proceed with
     Section 2.4.

     If you have installed QNX Momentics 6.x on Windows host, proceed with
     Section 2.5.

     If you have installed QNX Momentics 6.x on Solaris host, proceed with
     Section 2.6.

2.4. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on QNX 6.x host.

     If you have installed QNX Momentics 6.x on Windows host, skip this Section,
     and proceed with Section 2.5 instead.

     If you have installed QNX Momentics 6.x on Solaris host, skip this Section,
     and proceed with Section 2.6 instead.

     Before you begin, you must be logged in as superuser ('root'). 

2.4.1. Create 'diskonchip' sub-directory under directory /usr/local, and 
       change to this directory:

           # mkdir -p /usr/local/diskonchip
           # cd /usr/local/diskonchip

2.4.2. Copy archive diskonchip622-qnx6-x86.tar.gz to /usr/local/diskonchip 
       directory.

2.4.3. Unpack archive diskonchip622-qnx6-x86.tar.gz:

           # gunzip diskonchip622-qnx6-x86.tar.gz
           # tar -xvf diskonchip622-qnx6-x86.tar

2.4.4. Copy DiskOnChip driver 'devb-doc3' and formatter 'dformat3' to both /sbin 
       and /x86/sbin directories:

           # cp devb-doc3 /sbin
           # cp devb-doc3 /x86/sbin

           # cp dformat3 /sbin
           # cp dformat3 /x86/sbin

2.4.5. Set correct file permissions for 'devb-doc3' and 'dformat3':        

           # chmod 775 /sbin/devb-doc3 /x86/sbin/devb-doc3
           # chmod 775 /sbin/dformat3 /x86/sbin/dformat3

2.4.6. Copy "build" file 'bios.diskonchip.build' to both /boot/build and 
       /x86/boot/build directory:

           # cp bios.diskonchip.build /boot/build
           # cp bios.diskonchip.build /x86/boot/build

2.4.7. Create subdirectory 'diskonchip' under /usr/include, and copy all 
       DiskOnChip header files to /usr/include/diskonchip:

           # mkdir /usr/include/diskonchip
           # cp include/*.h /usr/include/diskonchip

2.4.8. Skip the rest of Chapter 2.

2.5. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on Windows host.

     If you have installed QNX Momentics 6.x on QNX 6.x host, skip this Section,
     and proceed with Section 2.4 instead.

     If you have installed QNX Momentics 6.x on Solaris host, skip this Section,
     and proceed with Section 2.5 instead.

2.5.1. Copy archive diskonchip622-qnx6-x86.tar.gz to temporary directory, and
       unpack it.

       ====> NOTE. You can use WinZip utility to open .tar.gz archive.

2.5.2. Copy DiskOnChip driver 'devb-doc3' and formatter 'dformat3' to 
       <QNXsdk>\target\qnx6\x86\sbin directory:

           # copy <tmp>\devb-doc3 <QNXsdk>\target\qnx6\x86\sbin
           # copy <tmp>\dformat3  <QNXsdk>\target\qnx6\x86\sbin

2.5.3. Copy "build" file 'bios.diskonchip.build' to 
       <QNXsdk>\target\qnx6\x86\boot\build directory

           # copy <tmp>\bios.diskonchip.build <QNXsdk>\target\qnx6\x86\boot\build

2.5.4. Create subdirectory 'diskonchip' under <QNXsdk>\target\qnx6\usr\include, 
       and copy all DiskOnChip header files there:

           # mkdir <QNXsdk>\target\qnx6\usr\include\diskonchip
           # copy <tmp>\include\*.h \usr\include\diskonchip

2.5.5. Skip the rest of Chapter 2.

2.6. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on Solaris host.

     If you have installed QNX Momentics 6.x on QNX 6.x host, skip this Section,
     and proceed with Section 2.4 instead.

     If you have installed QNX Momentics 6.x on Windows host, skip this Section,
     and proceed with Section 2.5 instead.

2.6.1. Copy archive diskonchip622-qnx6-x86.tar.gz to temporary directory, and
       unpack it:

           # gunzip <tmp>/diskonchip622-qnx6-x86.tar.gz
           # tar -xvf <tmp>/diskonchip622-qnx6-x86.tar

2.6.2. Copy DiskOnChip driver 'devb-doc3' and formatter 'dformat3' to 
       <QNXsdk>/target/qnx6/x86/sbin directory:

           # cp <tmp>/devb-doc3 <QNXsdk>/target/qnx6/x86/sbin
           # cp <tmp>/dformat3  <QNXsdk>/target/qnx6/x86/sbin

2.6.3. Copy "build" file 'bios.diskonchip.build' to 
       <QNXsdk>/target/qnx6/x86/boot/build directory

           # cp <tmp>/bios.diskonchip.build <QNXsdk>/target/qnx6/x86/boot/build

2.6.4. Create subdirectory 'diskonchip' under <QNXsdk>/target/qnx6/usr/include, 
       and copy all DiskOnChip header files there:

           # mkdir <QNXsdk>/target/qnx6/usr/include/diskonchip
           # cp <tmp>/include/*.h /usr/include/diskonchip


3. INSTALLING QNX 6.X TO THE DISKONCHIP
---------------------------------------

3.1. The easiest way to install QNX 6.x to DiskOnChip is to install QNX 6.x
     to target system's hard disk, plug DiskOnChip into the target system, boot 
     QNX 6.x from the hard disk, and copy QNX 6.x to DiskOnChip as explained
     below in this Chapter.

     ===> NOTE. You will need to go through this procedure only once, for the 
                very first DiskOnChip only. Once you have done this, you can 
                use M-Systems' utilities GETIMAGE.EXE and PUTIMAGE.EXE to copy
                contents of this DiskOnChip part to other DiskOnChip parts (this
                process is referred to as "DiskOnChip cloning"). GETIMAGE.EXE and
                PUTIMAGE.EXE utilities are part of DiskOnChip utility set for 
                DOS; they can be downloaded from M-Systems' web site 
                http://www.m-sys.com. M-Systems also offer specialized, low-cost
                DiskOnChip cloning equipment (known as "DiskOnChip GANG 
                Programmer") for automated DiskOnChip cloning. 

     ===> NOTE. The following DiskOnChip parts:

                    MD4331
                    MD4811
                    MD4832
                    MD5811
                    MD5832

                are shipped from the factory unformatted and without firmware. 
                Before you install QNX 6.x on these DiskOnChip parts, it's a good 
                idea to try booting DOS from these DiskOnChip parts on your QNX 
                target system. Appendix B explains how to do this.
     
3.2. Attach hard disk to QNX target system, install QNX 6.x onto this hard disk,
     and boot QNX target system from this hard disk.

3.3. Copy DiskOnChip driver 'devb-doc3' and formatter 'dformat3' to directory 
     /sbin on QNX target system. Make sure that both 'devb-doc3' and 'dformat3' 
     have correct access permissions:

         # chmod 775 /sbin/devb-doc3 /sbin/dformat3

3.4. If you will be plugging DiskOnChip into QNX target system using one of 
     M-Systems's DiskOnChip Evaluation boards, check Evaluation board's jumper
     settings, to find out which address DiskOnChip part(s) are mapped to. These
     jumper settings are usually listed on the back of Evaluation boards. By 
     default, most M-Systems' Evaluation boards map DiskOnChip to address 
     0xd0000 (in DOS notation, d000:0000).
    
3.5. Shutdown QNX target system, and plug DiskOnChip into it.

3.6. Boot QNX 6.x on the QNX target system from the hard disk, and login as 
     superuser ('root').

3.7. Start DiskOnChip driver:

          # devb-doc3

     After a short while, you should see the message similar to the one below:

          DiskOnChip driver, ver. 6.2.2
          Path=0 - M-Systems DOC
          target=0 lun=0 Direct-Access(0) - M-Systems DOC

     At this point you may need to hit <ENTER> to get back to shell's prompt.

     ===> NOTE. There are many configuration options that can be passed to 
                DiskOnChip driver via command line parameters. To see short 
                description of these options, execute:

                    # use devb-doc3

                The complete description of all these options can be found in
                Chapter 5.

3.8. List contents of /dev directory to find out what device name has been 
     assigned to DiskOnChip:

          # ls /dev/hd?

     The /dev/hd<digit> entry with the highest <digit> will be DiskOnChip's 
     device name. For example, if there is only one hard disk attached to target
     system, you will see entries /dev/hd0 and /dev/hd1 appearing in command's 
     output. In this case device name /dev/hd0 refers to the hard disk; 
     DiskOnChip's device name will be /dev/hd1. 

     It is assumed below that DiskOnChip's device name is /dev/hd1.

3.9. Clear DiskOnChip's partition table as shown below:

         # dd if=/dev/zero of=/dev/hd1 count=100

     ===> WARNING. Before executing the command above, verify that device name
                   /dev/hd1 indeed refers to DiskOnChip.
                   
3.10. Create QNX partition spanning entire DiskOnChip, mark this partition as 
      bootable, and write QNX loader to DiskOnChip by executing the following
      command sequence:

          # fdisk /dev/hd1 delete -a
          # fdisk /dev/hd1 add -s 1 qnx all
          # fdisk /dev/hd1 boot -s 1
          # fdisk /dev/hd1 loader

      ===> WARNING. Before executing the commands above, verify that device
                    name /dev/hd1 indeed refers to DiskOnChip.
                   
      Verify that contents of DiskOnChip's partition table are as expected:

          # fdisk /dev/hd1 show

3.11. Mount disk partitions from DiskOnchip as shown below:

          # mount -e /dev/hd1

      At this point, if you execute the following command:

          # ls /dev/hd*

      you should see device name /dev/hd1t77 appearing in the listing. This 
      device name referes to the newly created QNX partition on DiskOnChip.

3.12. Initialize QNX file system on QNX partition on DiskOnChip as shown below:

          # dinit -h /dev/hd1t77
          # dinit -hb /dev/hd1t77

     ===> WARNING. Before executing the command above, verify that device name
                   /dev/hd1 indeed refers to DiskOnChip.

3.13. Create directory /mnt/diskonchip:

         # mkdir -p /mnt/diskonchip

3.14. Mount QNX file system from QNX partition on DiskOnChip on /mnt/diskonchip
      directory as shown below:

         # mount /dev/hd1t77 /mnt/diskonchip

3.15. Create number of directories on DiskOnChip by executing command sequence 
      below:

         # mkdir /mnt/diskonchip/bin
         # mkdir /mnt/diskonchip/sbin
         # mkdir -p /mnt/diskonchip/lib/dll
         # mkdir -p /mnt/diskonchip/usr/lib
         # mkdir /mnt/diskonchip/usr/bin
         # mkdir /mnt/diskonchip/usr/sbin
         # mkdir /mnt/diskonchip/root

3.16. You will need to copy all files that are required by your QNX 6.x system,
      from directories on the hard disk to respective directories on DiskOnChip.
      If you are not sure which files are actually required, simply copy all of
      them:

         # cp /bin/* /mnt/diskonchip/bin
         # cp /sbin/* /mnt/diskonchip/sbin
         # cp -R /lib/* /mnt/diskonchip/lib
         # cp /usr/bin/* /mnt/diskonchip/usr/bin
         # cp /usr/sbin/* /mnt/diskonchip/usr/sbin

      Note that in this case the combined size of all the copied files will 
      come to 30 Mbytes, so it's a good idea to delete some of them right away. 
      For example, you will find many network drivers (with names starting with
      "devn"), video drivers (with names starting with "devg") and audio drivers
      (with names starting with "deva") in directory /mnt/diskonchip/lib/dll. If
      you system does not need these drivers, you can safely delete them. You
      can also delete all static libraries (files which names ending with ".a")
      from /mnt/diskonchip/lib, many utilities from /mnt/diskonchip/usr/bin etc.

3.17. You will need to build QNX 6.x bootimage for booting from DiskOnChip. 
      This could be done in two ways - from within QNX Momentics' "QNX Systems
      Builder" perspectiveby, or by running QNX 6.x utility 'mkifs' from 
      command line.

      Section 3.17.2 contains instructions on how to build QNX 6.x bootimage
      from within QNX Momentics' "QNX Systems Builder" perspective. 

      Section 3.17.1 contains instructions on how to build QNX bootimage by
      running 'mkifs' utility. This section applies to QNX 6.x self hosted 
      development systems only.

3.17.1. This Section explains how to build QNX 6.x bootimage from within 
        QNX Momentics' "QNX Systems Builder" perspective. 

        From QNX Momentics's menu, select "File->New->System Builder Project". 
        Name your project, click "Next", pick "Import Existing Buildfile" from 
        the popup list, and click "Next" again. Browse to 
        <QNXsdk>/target/qnx6/x86/boot/build directory, pick bios.diskonchip.build
        file, and click "Finish". QNX Momentics should now create new project, 
        and display it in the "Navigator" view. You can build this project by 
        clicking "Build System" icon (the one with the hammer) on the main 
        toolbar. The build process will create .ifs file in 
        <QNXsdk>/workspace/<project_name>/Images directory. This .ifs file is
        the QNX 6.x bootimage for booting from DiskOnChip.

        Skip to Section 3.17.3.

3.17.2. This Section explains how to build QNX 6.x bootimage by running 
        'mkifs' utility. It applies to QNX 6.x self hosted development systems
        only.

        In order to build QNX 6.x bootimage for booting from DiskOnChip, copy
        build file bios.diskonchip.build (provided as part of DiskOnChip driver
        distribution) to directory /x86/boot/build, change to /x86/boot directory,
        and run 'mkifs' utility:

            # cd /x86/boot
            # mkifs -v build/bios.diskonchip.build fs/bios.diskonchip.ifs

        The command above should create file /x86/boot/fs/bios.diskonchip.ifs,
        which is QNX 6.x bootimage for booting from DiskOnChip.

3.17.3. You need to transfer newly built QNX 6.x bootimage to temporary 
        directory on QNX target system (via FTP, for example), and copy it from
        there to file .boot in root directory on DiskOnChip:

         # cp /tmp/bios.diskonchip.ifs /mnt/diskonchip/.boot
         # rm /tmp/bios.diskonchip.ifs

3.18. Unmount DiskOnChip as shown below:

         # umount /mnt/diskonchip

3.19. Shutdown target system:

         # shutdown -f

3.20. Detach hard disk from the target system, power up target system, enter 
      BIOS setup, and disable all hard disks. Save changed BIOS settings, and
      exit BIOS setup. You should now see target system booting QNX 6.x from
      DiskOnChip.      


4. CONFIGURING DRIVER FOR VARIOUS TYPES OF HARDWARE INTERFACES TO DISKONCHIP
----------------------------------------------------------------------------

4.1. By default, DiskOnChip driver assumes that the following four conditions
     are all true:

         (a) DiskOnChip's pin A0 is connected to line A0 of the system's address
             bus.

         (b) It is possible to read/write individual 32-bit long word at any 
             long-word-aligned address within DiskOnChip memory window

         (c) It is possible to read/write individual 16-bit short word at any 
             short-word-aligned address within DiskOnChip memory window

         (d) It is possible to read/write individual byte at any address within
             DiskOnChip memory window

     While all four conditions are true in many cases, there are cases when some
     of these conditions are false. For example, if your particular board does
     not support 8-bit access to DiskOnChip (i.e. condition (d) is false), but 
     does support both 16-bit and 32-bit access (i.e. conditions (c) and (d) are
     true), you should instruct DiskOnChip driver not to use 8-bit access when it
     reads/writes to 8-bit-wide DiskOnChip registers. Instead, DiskOnChip driver
     should read register location as 16-bit short word, and then extract 8-bit 
     register contents from that short word. Similarly, when writing to 8-bit-wide
     DiskOnChip register, DiskOnChip driver should construct 16-bit short word,
     and then write it to register's location.

     If all conditions (a)-(d) above are correct for your particular board, skip
     the rest of Chapter 4.

     If any of the conditions (a)-(d) are false, you must tell DiskOnChip driver
     how DiskOnChip's pin A0 is connected to the system's address bus, and which
     types of access (out of those listed in conditions (b)-(d) above) within
     DiskOnChip memory window your particular board supports. This is 
     accomplished by passing "access=<mode>" option to DiskOnChip driver.

     The <mode> parameter is specified as 32-bit integer value in C-style hex 
     notation (for example, "mode=0x00000007"). The <mode> parameter consists of
     four bytes, which are used to specify access mode for up to four DiskOnChip
     "sockets" (see Section 1.3 for definition of term "socket"), with the least
     significant byte of <mode> applied to the first DiskOnChip "socket", and
     the most significant byte applied to forth DiskOnChip "socket". If value of 
     particular byte in <mode> parameter is zero, the access mode for respective
     DiskOnChip "socket" is left unchanged. 
     
     Each byte in <mode> parameter is individually constructed in two steps. 
     First, you select one of the following numerical constants that specify how
     DiskOnChip's pin A0 is connected to system's address bus:

         - if DiskOnChip's pin A0 is connected to line A0 of system's address
           bus, select value 0x0

         - if DiskOnChip's pin A0 is connected to line A1 of system's address 
           bus, select value 0x10
         
         - if DiskOnChip's pin A0 is connected to line A2 of system's address 
           bus, select value 0x20

     Then you specify all types of access within DiskOnChip memory window that
     your particular board supports, by adding their respective constants into
     <mode> byte:

         - if it is possible to read/write individual 32-bit long words to any 
           long-word-aligned address within DiskOnChip memory window, add 0x04
           to <mode> byte
            
         - if it is possible to read/write individual 16-bit short words to any
           short-word-aligned address within DiskOnChip memory window, add 0x02
           to <mode> byte

         - if it is possible to read individual byte from any address within
           DiskOnChip memory window, add 0x01 to <mode> byte

     By default, DiskOnChip driver sets access mode to 0x07070707, which means
     that for each out of four supported DiskOnChip "sockets", driver assumes 
     that DiskOnChip's pin A0 is connected to line A0 of system's address bus;
     board allows reading/writing 32-bit long words to long-word-aligned 
     addresses, 16-bit short words to short-word-aligned addresses, and bytes
     to anywhere within DiskOnChip memory window.

     Here is an example for the case when there is single DiskOnChip "socket" in
     the system; DiskOnChip's pin A0 is connected to line A0 of system's address
     bus; board supports reading/writing of 32-bit long words to 
     long-word-aligned addresses and 16-bit short words to short-word-aligned
     addresses within DiskOnChip memory windows, but does not allow access to
     individual bytes:

         # devb-doc3 doc access=0x00000006
 
     Note that "0x00000006" can be abbreviated to simply "0x6":

         # devb-doc3 doc access=0x6
 
     Here is more complex example for the case when there are four DiskOnChip
     "socket" in the system, with the following configurations:

         - DiskOnChip socket #0 has it's pin A0 is connected to line A2 of 
           system's address bus; board supports reading/writing of 32-bit long
           words to long-word-aligned addresses within DiskOnChip memory window,
           but does not allow neither 16-bit nor 8-bit access. The respective 
           byte in <mode> parameter for this "socket" should be specified as 
           0x24.

         - DiskOnChip socket #1 has it's pin A0 is connected to line A0 of 
           system's address bus; board supports reading/writing of 32-bit long
           words to long-word-aligned addresses, 16-bit short words to 
           short-word-aligned addresses, and bytes anywhere within DiskOnChip
           memory window. The respective byte in <mode> parameter for this 
           "socket" should be specified as 0x07 (which is driver's default 
           setting for access mode).

         - DiskOnChip socket #2 has it's pin A0 is connected to line A0 of 
           system's address bus; board supports reading/writing of 32-bit long
           words to long-word-aligned addresses and 16-bit short words to 
           short-word-aligned addresses, but does not allow 8-bit access within
           DiskOnChip memory window. The respective byte in <mode> parameter
           for this "socket" should be specified as 0x06.

         - DiskOnChip socket #3 has it's pin A0 is connected to line A0 of 
           system's address bus; board supports reading/writing of 32-bit long 
           words to long-word-aligned addresses, 16-bit short words to 
           short-word-aligned addresses, and bytes anywhere within DiskOnChip
           memory window. The respective byte in <mode> parameter for this 
           "socket" should be specified as 0x07 (which is driver's default 
           setting for access mode).

     In this case, <mode> parameter should be specified as '0x07060724'   

         # devb-doc3 doc access=0x07060724

     Note that since DiskOnChip "sockets" #1 and #3 have their bytes in <mode> 
     specified as 0x7 (which is driver's default setting for access mode), it
     is possible to specify these bytes as zero (which is interpreted as request
     to leave existing default access mode settings unchanged for these 
     "sockets"). In this case <mode> can be specified as 0x00060024, or simply
     as 0x60024:

         # devb-doc3 doc access=0x60024


5. DRIVER'S CONFIGURATION OPTIONS
---------------------------------

5.1. DiskOnChip driver features numerous command line option, which allows user
     to modify various aspects of driver's behavior. All these command line 
     options must be specified in driver's command line after 'doc' keyword,
     for example:

         # devb-doc3 doc addr=0xd0000

     Multiple options can be specified, in which case they should be separated
     by commas: 

         # devb-doc3 doc addr=0xd0000,chain=3

     DiskOnChip driver supports up to four DiskOnChip "sockets" (see Section 1.3
     for definition of term "socket"). It's possible to pass different values
     to "addr=<address>" and "passwd=<string>" options for different "sockets" 
     by specifying 'doc' keyword once for every "socket":
     
         # devb-doc3  doc addr=0xd0000  doc addr=0xd4000,passwd=abcdefgh

     All DiskOnChip driver's options other then "addr=<address>" and 
     "passwd=<string>", must be specified only once.

5.2. By default, DiskOnChip driver scans memory range [0xc8000..0xf0000] in the
     host address space, probing for DiskOnChip. There is a chance that these 
     memory probs might interfere with some other memory mapped devices (such 
     as older network cards or SCSI adapters). If the address of DiskOnChip is
     known in advance, it make sense to avoid these memory scanning by telling
     DiskOnChip driver to look for DiskOnChip at that address only. This address
     can be passed to DiskOnChip driver via 'addr=<address>' option. For 
     example, if it's known in advance that DiskOnChip is installed at address
     0xd0000, DiskOnChip driver can be started as follows:

         # devb-doc3 doc addr=0xd0000 

5.3. To facilitate troubleshooting of DiskOnChip problems, DiskOnChip driver 
     can be put into verbose mode by passing it 'verbose=<mode>' option. The 
     <mode> is specified as integer in C-style hex notation, and is interpreted
     as follows:

         bit 31         When set, this bit instructs driver to output messages 
                        showing status of the major driver operations (such as
                        detection of the DiskOnChip), as well as messages
                        indicating unrecoverable errors in the driver (such as
                        hardware errors). Normally this bit should always be set.

         bit 30         By default, when DiskOnChip driver encounteres recoverable
                        errors during it's operations (for example, single bit
                        data errors that are common to NAND flash), it silently
                        fixes these errors (for example, by using DiskOnChip's
                        ECC hardware to fix these single bit flash data errors).
                        This bit, when set, instructs DiskOnChip driver to output
                        messages indicating that such recoverable errors were
                        encountered and fixed by the driver. Normally this bit
                        should be cleared.
 
         bit 29         When set, this bit causes DiskOnChip driver to output
                        messages that indicate progress of every DiskOnChip
                        operation. Setting this bit makes driver's output extremely
                        verbose. Normally this bit should be cleared.

         bits 28..16    These bits are ignored.

         bits 15...0    DiskOnChip driver consists of various software components
                        which are responsible for handling various aspects of 
                        DiskOnChip operations. These bits allow user to turn on 
                        or off messages on per components basis. Normally these
                        bits are all set.
                        
     Usually <mode> is specified as 0x8000ffff:

         # devb-doc3 doc verbose=0x8000ffff

     in which case DiskOnChip driver will issue few messages indicating status
     of it's major operations (such as detection of the DiskOnChip), and report 
     any unrecoverable errors (such as hardware errors), if such errors will
     ever arise.

5.4. To configure DiskOnChip driver for various types of hardware interfaces to
     DiskOnChip, use 'access=<mode>' option. See Chapter 4 for more details
     regarding this option.

5.5. DiskOnChip parts provide user with option to write- and/or read- protect
     individual "disks" (see Section 1.4 for definition of term "disk"). To get 
     access to such protected "disks", user has to specify 8-character password
     via 'passwd=<string>' option. Here is example for the common case when there
     is only one "disk" on DiskOnChip "socket", protected by password "12345678":

         # devb-doc3 doc passwd=12345678

     Here is example for more complex case, when there are four "disks" on
     DiskOnChip "socket"; second and third "disks" are protected by passwords
     "12345678" and "abcdefgh" respectively; first and forth disks are
     not password-protected:

         # devb-doc3 doc passwd,passwd=12345678,passwd=abcdefgh,passwd

5.6. DiskOnChip driver maintains internal lookup tables in RAM in order to
     improve it's I/O performance. However, amount of RAM taken by these
     tables could be significant in cases of high capacity DiskOnChips.
     To reduce driver's memory footprint (at the expense of driver's I/O
     performance), use 'tabsize=<number> option. The <number> is specified
     as integer in C-style hex notation, and is interpreted as follows:

         bits 31..24     TrueFFS "handle" identifying socket/disk. In case
                         when there is single DiskOnChip in the system, these
                         bits are all set to zero.

         bits 23..16     These bits are ignored.

         bits 15...0     Size of driver's RAM tables specified as number
                         of entries in these tables. By default, DiskOnChip
                         driver allocates RAM tables large enough to contain
                         4095 entries (which is the maximum value supported 
                         by current version of DiskOnChip driver). 

     The example below assumes that there is single DiskOnChip in the
     system, and instructs DiskOnChip driver to reduce size of its' RAM
     tables to 256 entries (hex 0x100): 

         # devb-doc3 doc tabsize=0x00000100

     ===> NOTE. For performance reasons, it's not recommended to reduce the
                size of driver's tables to less then 256 entries.  

5.7. By default, DiskOnChip driver favors I/O performance over I/O determinism.
     To configure driver to favor I/O determinism over I/O performance, use
     'chain=<number>' and 'nowait' options. Both of these options are described
     in Chapter 9. 

5.8. To request DiskOnChip driver to perform verification of 'write' operations,
     use 'verify=<mode>" option. The parameter <mode> is specified as integer in
     C-style hex notation. Only least significant byte is actually used in <mode>
     parameter; it's two least significant bits applied to first DiskOnChip 
     "socket", next two bits applied to second "socket" etc. When particular 
     pair of bits is specified as zero, verification mode of the corresponding
     "socket" is left unchanged. Non-zero value in bits pair specifies level of
     verification to be done on respective "socket", with '1' for minimal 
     verification mode and '3' for maximum verification. Driver's default setting 
     for <mode> is 2 (i.e. medium) for each of four sockets. Here is example for
     the system with single DiskOnChip "socket" configured for maximum 
     verification of 'write' operations:

         # devb-doc3 doc verify=0x3

     Here is more complex example for the system with four DiskOnChip "sockets".
     First and third "sockets" are configured for maximum verification; second
     "socket" configured for medium verification, and verification setting for
     forth "socket" is left unchanged. The <mode> parameter in this case is
     specified as 0x3B (which is 00111011 in binary notation): 

         # devb-doc3 doc verify=0x3B
 
      ===> NOTE. Setting verification mode to maximum value could negatively
                 impact DiskOnChip's 'write' performance.

5.9. DiskOnChip driver provides two options for setting verification mode for 
     'write' operations which driver performs internally when user issues devctl()
     calls to DiskOnChip (see Chapter 7). These options are "bverify=<mode>" and
     "vother=<mode>". In both cases, two least sigificant bits in <mode> 
     parameter specify verification mode, with '1' for minimal verification
     and '3' for maximum verification. These options are provided for completeness
     only; rarely there is a reason for changing driver's default settings for
     these options.

5.10. When starting, DiskOnChip driver, perform quick check of low-level flash
      format on the DiskOnChip. To instruct DiskOnChip driver to perform more
      thorough checking of low-level format during startup, use "chkdsk" option:
      
         # devb-doc3 doc chkdsk
       
      ===> NOTE. The "chkdsk" option could make driver's startup time longer.

5.11. By default, DiskOnChip driver moves data to/from DiskOnChip in 32-bit long
      words. While most of the boards can reliably translate 32-bit access
      cycles into correct sequence of 8-bit cycles, some boards can't do it 
      reliably. To make DiskOnChip driver working with these boards, use option
      '8bit'. This option instructs DiskOnChip driver to always move data to/from
      DiskOnChip in bytes rather then 32-bit long words:

         # devb-doc3 doc 8bit
  
5.12. By default, DiskOnChip driver's I/O thread(s) run at priority 21, which
      is higher then priorities of many other threads in the system (for example,
      shell by default runs priority 10). As amount of DiskOnChip I/O increases,
      DiskOnChip driver's I/O thread(s) start consuming larger share of CPU time,
      leaving less CPU time available for other threads. This causes system to
      appear sluggish in responding to events such as user input or screen 
      update. To avoid this problem, user can instruct DiskOnChip driver to
      run it's I/O thread(s) at different priority by pasing it option 
      "prio=<priority>". For example, to tell DiskOnChip to run it's I/O threads
      at priority 9:

         # devb-doc3 doc addr=0xc0000000,prio=9
  
      ===> NOTE. Priorities in QNX6 system vary from zero (lowest) to 255
                 (highest). 

      After DiskOnChip driver has been started, priorities of all of it's
      threads can be changed using 'renice' command. 

5.13. By default, DiskOnChip driver terminates when it receives some signals
      (for example, SIGTERM). To tell it to ignore these signals, user can
      pass driver "nosig=<signal_number>" options, where <signal_number> is
      specified as in <signal.h> header. For example, to tell driver to
      ignore SIGTERM (whose numerical value is specified in <signal.h> as 
      '15'):
 
         # devb-doc3 doc addr=0xc0000000,nosig=15

      ===> NOTE. The SIGKILL signal cannot be ignored.


6. FORMATTING DISKONCHIP
------------------------

     ===> WARNING. ALL USER DATA IS DELETED WHEN DISKONCHIP IS FORMATTED !

6.1. In case if DiskOnChip has been mounted previously, it must be unmounted
     before formatting, and DiskOnChip driver must be shut down. Assuming that
     DiskOnChip has been mounted as described in Chapter 3, it can be 
     unmounted as follows:

         # umount /mnt/diskonchip

     DiskOnChip driver can be shut down as follows:

         # slay devb-doc3

6.2. DiskOnChip is formatted with 'dformat3' utility. In most cases, DiskOnChip 
     can be formatted by simply executing 'dformat3' without any arguements:

         # dformat3

     It is recommended to reboot system after DiskOnChip has been formatted.

6.3. By default, 'dformat3' scans memory range [0xc8000..0xf0000] in host address
     space, probing for DiskOnChip. These memory probs can potentially interfere
     with other memory mapped devices (such as older network cards or SCSI 
     adapters). If address of DiskOnChip is known in advance, it makes sense to
     avoid memory scanning by telling DiskOnChip driver to look for DiskOnChip
     at this particular address only. The address can be passed to 'dformat3' 
     via '-w:<address>' command line option. For example, if it's known in
     advance that DiskOnChip is installed at address 0xd0000:

         # dformat3 -w:d0000 

     ===> NOTE. Note that address is specified as 'd0000', not '0xd0000'. This
                is different from 'devb-doc3' driver (which expects address
                to be specified as "0xd0000", see Section 5.1). M-Systems'
                DOS tool DFORMAT.EXE also expects address to be specified
                in this format (see Appendix B).

6.4. By default, 'dformat3' accesses DiskOnChip in 32-bit mode. While most of
     x86 boards are capable of reliably translating 32-bit cycles into
     sequence of 8-bit cycles, there is a chance that some older boards won't
     be able to do so. In this case command line option '-8' must be used, 
     to force 'dformat3' to access DiskOnChip strictly in 8-bit mode:

         # dformat3 -8
  
6.5. To facilitate troubleshooting of DiskOnChip problems, 'dformat3' can be put
     into verbose mode by specifying '-v:<mode>' command line option. The <mode>
     is specified as integer in C-style hex notation, and is interpreted as
     described in Section 5.3. Usually <mode> is specified as 0x8000ffff:

         # dformat3 -v:0x8000ffff 

     in which case 'dformat3' will issue few messages indicating status of
     it's major operations (such as detection of the DiskOnChip), and report 
     any unrecoverable errors (such as hardware errors), if such errors will
     ever arise.

6.6. By default, 'dformat3' uses the entire flash media (with exception of 
     approximately 150 KBytes at the beginning of the flash media which is 
     occupied by DiskOnChip's firmware) when formatting DiskOnChip. To instruct
     'dformat3' to skip region of flash memory immediately following the firmware
     area, and start formatted data area at particular offset from firmware area, 
     use '-b' command line option. For example, to reserve 2 MBytes of flash 
     space, and start formatting at 2 MByte offset from firmware area:

         # dformat3 -b:2m

     In this case, the media layout will be as follows: there will be 150 KBytes
     at the beginning of the media that is occupied by DiskOnChip's firmware,
     then 2 MBytes of unformatted flash space; the the rest of the media will
     be formatted for file system storage.

     Here is another example of reserving 700 KBytes of flash space:
 
         # dformat3 -b:700k

6.7. DiskOnChip allows user to create multiple "disks" on single physical 
     DiskOnChip "socket" (see Chapter 1 for definition of terms "socket" and 
     "disk"). This can be achieved by using '-d' command line option. For 
     example, if you want to create three "disks" on DiskOnChip 64 MByte
     part, with first "disk" being 5 MBytes, second "disk" being 10000 
     KBytes, and third "disk" spanning the rest of the media, you should 
     execute:
     
         # dformat3 -d:5m,10000k,0

     In the example above, 'm' denotes MBytes, 'k' denotes KBytes, and final
     zero is interpreted as "remaining part of the media". 

6.8. DiskOnChip provides user with the option to write- and/or read- protect
     individual "disks" (see Chapter 1 for definition of term "disk"). While 
     current version of 'dformat3' does not allow user to specify read- or 
     write- protection for "disks" that it creates, it does allow user
     to turn off read- and write- protection on existing "disks" (normally
     created by M-Systems' DOS tool DFORMAT.EXE). In order to turn off
     read- or write- protection on every such "disk", user has to specify 
     8-character password via '-p' command line option. Here is example for 
     common case when there is only one read- or write- protected "disk" on 
     DiskOnChip:

         # dformat3 -p:12345678

     where "12345678" is the password. 

     Here is example for more complex case, when there are four "disks" on
     DiskOnChip, with second and third "disks" protected by passwords 
     "12345678" and "abcdefgh" respectively, and first and forth disks 
     unprotected:

         # dformat3 -p:,12345678,abcdefgh,

6.9. To configure 'dformat3' utility for various types of hardware interfaces to
     DiskOnChip (see Chapter 4), use '-a:<mode>' command line option. The <mode>
     parameter is specified in exactly the same fashion as in case of DiskOnChip
     driver's "access=<mode>" option (see Chapter 4). For example, in case when
     DiskOnChip's pin A0 is connected to line A0 of system's address bus; board 
     supports reading/writing of 32-bit long words to long-word-aligned addresses
     and 16-bit short words to short-word-aligned addresses within DiskOnChip
     memory windows, but does not allow access to individual bytes, you would 
     execute:

         # dformat3 -a:0x6
 

7. ACCESS TO DISKONCHIP'S ADVANCED FUNCTIONS
--------------------------------------------

     DiskOnChip driver provides user with full access to all DiskOnChip Advanced
     Functions. All Advanced Functions are accessed via devctl() calls to the
     DiskOnChip driver. Note that DiskOnChip driver must be running at the moment
     when application issues devctl() call to it.

     The rest of this Chapter explain how to access DiskOnChip Advanced Functions
     via devctl() calls to DiskOnChip driver. Appendix A contains complete code 
     example of accessing FL_IOCTL_GET_INFO DiskOnChip Advanced Function via
     devctl() call to the DiskOnChip driver.

     This version of DiskOnChip driver provides user with access to the
     following DiskOnChip Advanced Functions:

         FL_IOCTL_GET_INFO
         FL_IOCTL_DEFRAGMENT
         FL_IOCTL_WRITE_PROTECT
         FL_IOCTL_MOUNT_VOLUME
         FL_IOCTL_BDK_OPERATION
         FL_IOCTL_BINARY_HW_PROTECTION
         FL_IOCTL_BDTL_HW_PROTECTION
         FL_IOCTL_OTP
         FL_IOCTL_CUSTOMER_ID
         FL_IOCTL_UNIQUE_ID
         FL_IOCTL_NUMBER_OF_PARTITIONS
         FL_IOCTL_INQUIRE_CAPABILITIES
         FL_IOCTL_DEEP_POWER_DOWN_MODE
         FL_IOCTL_DELETE_SECTORS
         FL_IOCTL_READ_SECTORS
         FL_IOCTL_WRITE_SECTORS
         FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES
         FL_IOCTL_VERIFY_VOLUME
         FL_IOCTL_EXTENDED_WRITE_IPL
         FL_IOCTL_IS_QUICK_MOUNT_VALID
         FL_IOCTL_WRITE_QUICK_MOUNT_INFO
         FL_IOCTL_CLEAR_QUICK_MOUNT_INFO
         FL_IOCTL_COMPLETE_OPERATION

     ===> NOTE. Complete description of all DiskOnChip Advanced Functions, along
                with their associated data structures, can be found in M-Systems'
                "DiskOnChip TrueFFS 6.x Software Development Kit. Developer's 
                Guide" manual.
      
7.1. In order to issue devctl() call to DiskOnChip driver, you will need to open 
     DiskOnChip special device file as shown below:


         #include <sys/types.h>
         #include <sys/stat.h>
         #include <fcntl.h>

         /* DiskOnChip device name (see NOTE below) */

         char devname[] = "/dev/hd1";

         int fd = open (devname, O_RDONLY);

         if (fd == -1) 
         {
             /* error */
         }


     ===> NOTE. The code fragment above assumes that DiskOnChip's device name is 
                "/dev/hd1". This is correct only if target system boots from the
                hard disk (which is assigned device name "/dev/hd0"), and starts
                DiskOnChip driver afterward. Normally, however, taget system 
                boots from DiskOnChip, in which case DiskOnChip is assigned 
                device name "/dev/hd0". In this case the code fragment above 
                must be changed to use "/dev/hd0" name instead of "/dev/hd1".

7.2. You will need to allocate instance of 'struct flIoctlGeneric'. This data 
     structure is defined in file /usr/include/diskonchip/fldrvnto.h. Note that 
     this data structure is fairly large (3 KByte), so it makes sense to allocate
     it dynamically via malloc() rather then allocate it in the stack:
     

         #include <malloc.h>
         #include <diskonchip/fldrvnto.h>

         flIoctlGeneric * req;

         req = malloc( sizeof(*req) );

         if (req == NULL) 
         {
             /* error */
         }
         

7.3. Consult "DiskOnChip TrueFFS 6.x Software Development Kit. Developer's 
     Guide" manual (included as part of DiskOnChip driver distribution) to find
     out which input and output data structures must be passed to DiskOnChip
     driver when accessing particular DiskOnChip Advanced Function, and identify
     fields in 'flIoctlGeneric' data structure (defined in header 
     /usr/include/diskonchip/fldrvnto.h) that corresponds to these input and 
     output data structures. For example, in case of FL_IOCTL_DEFRAGMENT Advanced
     Function, you will need to pass 'flDefragInput' as input data structure
     and 'flDefragOutput' as output data structure, so you will use 
     'flIoctlGeneric.in.defragInput' and 'flIoctlGeneric.out.defragOutput'
     fileds.

7.4. Some of the DiskOnChip Advanced Function require user to pass data buffers
     along with an input data structure. For example, when using 
     FL_IOCTL_BDK_OPERATION Advanced Function, user must passed data buffer
     along with the input data structure 'flIoctlGeneric.in.bdkOperationInput'.
     In all such cases, user's data must be copied into 'flIoctlGeneric.uData'
     input/output buffer. The size of 'flIoctlGeneric.uData' input buffer is 
     currently 4 KBytes.

     ===> NOTE. Contents of 'flIoctlGeneric.uData' input buffer are not 
                preserved during devctl() call.
     
7.5. To access particular DiskOnChip's Advanced Function, issue devctl() call to
     DiskOnChip driver. For example, to access FL_IOCTL_GET_INFO Extended 
     Function:


         #include <sys/types.h>
         #include <unistd.h>
         #include <devctl.h>
         #include <diskonchip/fldrvnto.h>

         /* 'fd' was obtained in Section 7.1 */

         /* 'req' was allocated in Section 7.2, and initialized in Section 7.3 */

         /* issue device control call */
 
         int  dcmd = __DIOTF( _DCMD_TFFS, FL_IOCTL_GET_INFO, (*req) );

         if( devctl(fd, dcmd, req, sizeof(*req), NULL) != EOK ) 
         {
             /* error */
         }


7.6. If particular DiskOnChip Advanced Function returns status code, you should 
     check it. For example, FL_IOCTL_GET_INFO Extended Function returns status 
     code in field 'flIoctlGeneric.out.diskInfoOutput.status':


         #include "diskonchip/fldrvnto.h"

         if (req.out.diskInfoOutput.status != flOK) 
         {
             /* error, see error codes in diskonchip/flstatus.h */
         }

7.7. Some of DiskOnChip Advanced Functions (for example, FL_IOCTL_BDK_OPERATION), 
     in addition to returning output data structure in the respective field in 
     structure 'flIoctlGeneric', also return up to 4 KBytes of data. In all such
     cases, this data is returned in 'flIoctlGeneric.uData' buffer.


8. CONSIDERATIONS RELATED TO UNEXPECTED POWER SHUTDOWNS
-------------------------------------------------------

8.1. There are two levels of format on DiskOnChip - low level (physical) format,
     and high level (file system) format. 

     The low level (physical) format is M-Systems' proprietary, DiskOnChip
     specific format, which is created and maintained by DiskOnChip driver. 
     M-Systems guarantees that under no circumstances low level (physical) format
     will be corrupted by unexpected system shutdown. DiskOnChip driver's flash
     management algorithms have been carefully designed and thoroughly tested to 
     make sure that physical flash format is consistent at any given point of
     time.
    
     The high level (file system) format is common to all block devices that are 
     attached to QNX file system (DiskOnChip, ATA/IDE/SCSI disks etc.). The high
     level (file system) level format is maintained by QNX file system. It is
     completely up to QNX file system and QNX disk I/O manager 'io-blk' to keep
     high level disk format consistent. If your system is frequently subjected to
     power outages and unexpected shutdowns, you may consider passing option
     "commit=high" to io-blk:

         # devb-doc3 blk commit=high
        
     All the options following "blk" keyword will be passed along to QNX disk
     I/O manager 'io-blk'. These io-blk parameters can be combined with DiskOnChip
     options (following "doc" keyword), as shown below:

         # devb-doc3 doc addr=0xd0000,verbose=0x8000ffff blk commit=high

     For more details regarding 'io-blk' options, see 'io-blk' entry in QNX 6.x
     "Utilities Reference" manual.


9. CONSIDERATIONS RELATED TO I/O DETERMINISM
--------------------------------------------

9.1. It's well known fact that flash blocks can't be written to without been 
     erased first. The process of erasing of previously used ("dirty") flash 
     blocks is usually referred to as "flash garbage collection". DiskOnChip 
     driver is based on flash management algorithms that take care of this issue
     automatically, during the course of sector write operation, whenever driver
     runs out of the "clean" space on flash media. Obviously, the need to erase
     "dirty" flash blocks makes sector write operation longer. While for many 
     applications fact that length of sector write operation fluctuates isn't
     really a concern, some application could have special requirements regarding
     deterministic timing of disk 'write' operations. While it's not possible 
     to make every DiskOnChip 'write' operation to take exactly the same amount
     of time, it is possible to substantially reduce fluctuation in the duration
     of 'write' operations. The rest of this Chapter describes driver's 
     configuration options that can be used for this purpose. 

9.2. Default configuration of DiskOnChip driver allows for up to 7 flash blocks
     to be erased during single sector 'write' operation. With average duration
     of flash erase operation in case of DiskOnChip been about 2.5 milliseconds,
     single sector 'write' operation (which is normally completed within 
     1 millisecond) could occasionally spend up to 20 milliseconds waiting for
     all these flash blocks to be erased. For most of the systems such 
     fluctuation in duration of sector 'write' operations is barely noticeable,
     but if for some reason it is undesirable in case of your particular system,
     user could reduce it by lowering the limit on number of flash blocks that
     can be erased during single sector 'write' operation. This limit can be 
     specified via driver's 'chain=<number>' command line option. For example, 
     to make sure that every sector 'write' operation is completed within 
     10 milliseconds, user could start DiskOnChip driver as shown below:

         # devb-doc3 doc chain=2

     The minimal value for 'chain' option is 2, maximum is 14.
 
     ===> NOTE. This option can cause substantial decrease in DiskOnChip's
                'write' performance. 

9.3. Whenever DiskOnChip driverv receives request to write one or more sectors
     to DiskOnChip, it executes number of flash operations in order to write
     contents of these sectors on the media. By default, DiskOnChip driver 
     always waits for all such flash operations to complete before informing 
     system about the completion of sector 'write' operation. It is possible
     to configure driver to initiate flash operations on the media and 
     then, without waiting for these operations to actually complete, report
     completion of sector write operation to the system. To do that, user
     has to specify 'nowait' command line option: 
     
         # devb-doc3 doc nowait
     
     ===> NOTE. This option usually has only marginal effect on driver's I/O
                perfromance.


10. KNOWN LIMITATIONS
---------------------

10.1. If hardware write protection is activated on one of DiskOnChip "disks" (see
      Section 1.4 for definition of term "disk"), but this disk was not mounted as
      write protected, then attempt to write data to this disk will not produce 
      immediate error message from DiskOnChip driver. This is caused by the fact
      that data is usually held in disk cache for a while, and written to the disk
      with some delay.
 
10.2. Read- and write-protected "disk" on DiskOnChip must not use string 
      "00000000" as password.

10.3. While current version of 'dformat3' allows user to create multiple "disks"
      on DiskOnChip, it does not allow user to specify write- and read- protection
      for these "disks" - all "disks" are created writeable and readable. If you
      want to write- or/and read- protect these "disks", use M-Systems' DOS
      tool DFORMAT.EXE  for this purpose. This utility is part of M-Systems'
      utility set for DOS; it can be downloaded from M-Systems' web site 
      http://www.m-sys.com.


11. TROUBLESHOOTING
-------------------

11.1. If you are experiencing problems with DiskOnChip, run DiskOnChip driver 
      with 'verbose=<mode>' option (see Section 5.3). This will cause DiskOnChip
      driver to issue messages indicating status of it's major operations (such
      as detection of the DiskOnChip), and report any unrecoverable errors 
      (such as hardware errors), if such errors will ever arise.

11.2. DiskOnChip driver cannot detect DiskOnChip. 

      Possible solutions:

         - Check if DiskOnChip is properly inserted in it's socket.

         - If you are using DiskOnchip adapter, check adapter's jumper 
           settings.
           
         - Check BIOS setup, to verify that there is no memory shadowing in 
           the memory region that DiskOnChip is mapped to.

11.3. Unexpected system shutdown has corrupted file system on DiskOnChip. 

      Possible solutions: 

         - Run standard QNX 6.x 'chkfsys' utility to repair the damaged file
           system. 

         - Try passing options to DiskOnChip driver that are described in
           Chapter 8 

11.4. System becomes sluggish and unresponsive when doing a lot of DiskOnChip
      I/O.

      Possible solutions: see Section 5.12.


12. CONTACT INFORMATION
-----------------------

    North America:  

        M-Systems Inc.8371 Central Ave, Suite A Newark CA 94560
        Phone: 1-510-494-2090
        Fax:   1-510-494-5545

    Japan:

        M-Systems Japan Inc. Arakyu Bldg., 5F 2-19-2 Nishi-Gotanda Shinagawa-ku, Tokyo 141-0031
        Phone: 81-3-5437-5739
        Fax:   81-3-5437-5759

    Taiwan:

        M-System Asia Ltd. 11F-2, No. 81, Sec.2, Chang Teh Rd. Taipei, Taiwan
        Phone: 886-2-2550-1741
        Fax:   886-2-2550-1745  

    Europe:

        M-Systems Ltd. Central Park 2000 7 Atir Yeda St. Kfar Saba 44425, Israel
        Phone: 972-9-764-5000
        Fax:   972-3-548-8666

    U.K.:

        M-Systems UK Ltd.PO Box 20 Chalgrove SPDO OX44 7YP
        Phone: 44-1865-891-123
        Fax:   44-1865-891-391

    Internet: http://www.m-sys.com
    E-mail: info@m-sys.com

    Please checkout our web site for the latest driver versions and updated 
    information.     


APPENDIX A. EXAMPLE OF ACCESS TO DISKONCHIP'S ADVANCED FUNCTIONS
----------------------------------------------------------------

    /*
     *                                                                          
     *  Obtain DiskOnChip information using FL_IOCTL_GET_INFO Advanced Function.
     *                                                                          
     *  The routine below obtains and displays some DiskOnChip info. 
     *
     *  ===> NOTE. The code fragment below uses "/dev/hd1" as DiskOnChip's
     *             device name. This is appropriate if target system boots
     *             from the hard disk, and starts DiskOnChip driver afterwards.
     *             In case if system boots from DiskOnChip, you should change
     *             device name from "/dev/hd1" to "/dev/hd0".
     *                                                                          
     */
    
    #include <sys/types.h>
    #include <sys/qioctl.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <devctl.h>
    #include <diskonchip/fldrvnto.h>
    
    int example (void)
    {
        flIoctlGeneric   * req;
        VolumeInfoRecord * info;
        int                fd;
        int                dcmd;
        int                status;

        /* DiskOnChip device name - see NOTE above */

        char               devname[] = "/dev/hd1"; 
    
        /* open DiskOnChip device */
    
        fd = open (devname, O_RDONLY);
    
        if (fd == -1) 
            return -1;      /* error: can't open device */

        /* allocate flIoctlGeneric input/output structure */

        req = malloc( sizeof(*req) );

        if (req == NULL)
            return -1;      /* error: can't allocate flIoctlGeneric structure */

        /* 
         * There is no input data structure to pass to DiskOnChip driver.
         * Output structure will be placed in 'req->out.diskInfoOutput'.
         */
            
        /* issue devctl() call to DiskOnChip driver */

        dcmd = __DIOTF( _DCMD_TFFS, FL_IOCTL_GET_INFO, (*req) );
 
        status = devctl (fd, dcmd, req, sizeof(*req), NULL);
    
        /* check status code returned by devctl(0 call */

        if (status == -1) 
            return -1;      /* error in devctl() */
    
        /* check status code of DiskOnChip Advanced Function */
    
        if (req->out.diskInfoOutput.status != flOK) 
            return -1;      /* error in Advanced Function */
    
        /* display info received from DiskOnChip Advanced Function */
    
        info = &req->out.diskInfoOutput.info;
    
        printf ("DiskOnChip info\n");
        printf ("\twindow linear address                 : 0x%lx\n",       info->baseAddress);
        printf ("\tcontroller type                       : 0x%x ",   (int) info->DOCType);

        switch( (int) info->DOCType ) 
        {
            case FL_DOC_OREN:    printf( "(DiskOnChip MD24xx)\n" );             break; 
            case FL_MDOC512_G3:  printf( "(DiskOnChip G3 MD48xx 512 MBit)\n" ); break; 
            case FL_MDOC256_P3:  printf( "(DiskOnChip P3 MD58xx 256 Mbit)\n" ); break; 
            default:             printf( "\n" );                             break;
        }

        printf ("\tflash media type                      : 0x%x\n",        info->flashType);
        printf ("\tdisk's physical size                  : %ld bytes\n",   info->physicalSize);
        printf ("\ttotal size of all binary partition(s) : 0x%lx bytes\n", info->bootAreaSize);
        printf ("\ttotal sectors on disk                 : %ld\n",         info->logicalSectors);
    
        close (fd);
    
        return 0;
    }
    
    
APPENDIX B. BOOTING DOS FROM DISKONCHIP
---------------------------------------

B.1. Download the latest version of DiskOnChip tools for DOS from M-Systems' 
     web site http://www.m-sys.com (you will need version 6.2.2 or later
     of these tools).

B.2. Prepare DOS boot floppy. 

B.3. Copy M-Systems DFORMAT.EXE tool and DiskOnChip firmware file to DOS 
     floppy.

     ===> NOTE. DiskOnChip firmware file is part of M-Systems tools for DOS. It 
                usually has filename DOC<ver>.EXB, where <ver> is the version
                of DOS tools (for example, DOC622.EXB). You must use firmware
                version 6.2.2 or higher.

B.4. Boot target system with DOS from DOS floppy.

B.5. Re-format DiskOnChip using M-Systems' DFORMAT.EXE tool, as shown below:

         DFORMAT /WIN:<address> /S:<firmware> /Y

     where:

         <address>  is base address of DiskOnChip (i.e. d0000, d8000 etc.).
         <firmware> is DiskOnChip firmware file (for example, DOC622.EXB).

     For instance, if DiskOnChip is mapped to address d0000, and you are 
     using DOC622.EXB firmware file, you execute the following command:

         DFORMAT /WIN:d0000 /S:DOC622.EXB /Y

     ===> NOTE. If you don't know which address DiskOnChip is installed at,
                you can find it out by running DINFO.EXE utility.

B.6. Power off target system.

B.7. Power on target system, enter BIOS setup, and disable all hard disks.
     Exit BIOS setup, and continue booting from DOS floppy.

B.7. Once target system has booted, you should see DiskOnChip installed at
     drive letter C:

B.8. Execute the following command:

         SYS C:

     to transfer DOS boot files to DiskOnChip.

B.9. Remove DOS floppy from the drive, and reboot system. 

B.10. You should now see DOS booting from the DiskOnChip.

B.11. Power off target system, power it on, enter BIOS setup and undo the changes
      that you have ade in Section B.7.



