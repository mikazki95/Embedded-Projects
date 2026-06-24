
 ********************************************************************************
 *                                                                              *
 *  DiskOnChip(R) driver for QNX-6.x (x86 processors).                          *
 *                                                                              *
 *  Version 5.1.4p3                                                             *
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
 *      MD22xx                                                                  *
 *      MD28xx                                                                  *
 *      MD33xx                                                                  *
 *      MD38xx                                                                  *
 *                                                                              *
 *  If you are using DiskOnChip parts of different types, for example any of    *
 *  the following:                                                              *
 *                                                                              *
 *      MD24xx                                                                  *
 *      MD43xx                                                                  *
 *      MD48xx                                                                  *
 *      MD58xx                                                                  *
 *                                                                              *
 *  this version of DiskOnChip driver will *NOT* work with these parts. You     *
 *  must use DiskOnChip driver version 6.x with all such parts. You can         *
 *  download latest 6.x version (6.2.2 at the time of writing) of DiskOnChip    *
 *  driver for QNX 6 from M-Systems web site http://www.m-sys.com.              *
 *                                                                              *
 ********************************************************************************
    

VERSION MARK
------------

  Version 5.1.4p3  September 01, 2004
    

GENERAL
--------

  This directory contains QNX 6.x device driver and utilities for the following
  members of DiskOnChip product line:

      MD22xx     
      MD28xx     
      MD33xx     
      MD38xx     

  If you are using DiskOnChip parts of different types, for example any of the
  following:

      MD24xx     
      MD43xx     
      MD48xx     
      MD58xx     

  this version of DiskOnChip driver will NOT work with these parts. You must
  use DiskOnChip driver version 6.x with all such parts. You can download
  the latest 6.x version (6.2.2 at the time of writing) of DiskOnChip driver
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

1.1. When used under QNX, DiskOnChip is managed by dedicated device driver 
     'devb-doc'. DiskOnChip driver is normally used with QNX file system.

1.2. DiskOnChip driver uses term "socket" to refer to either of the following:

         - single DiskOnChip Millennium Plus or up to 4 cascaded parts
         - single Mobile DiskOnChip part or up to 4 cascaded parts
         - single DiskOnChip 2000 TSOP or up to 4 cascaded parts
         - single DiskOnChip Millennium or up to 4 cascaded parts
         - single DiskOnChip 2000 part (DIP/DIMM)

       ====> NOTE. Term 'cascaded' refers to the parts which all share the 
                   same address in the host's address space, and form a single
                   continuous flash media of capacity equal to the sum of 
                   capacities of individual cascaded parts.

     The DiskOnChip driver supports up to 4 "sockets", all possibly of different
     types / cascade configurations / capacities.

1.3. When working with the following DiskOnChip parts:

     - DiskOnChip Millennium Plus
     - Mobile DiskOnChip
     - DiskOnChip 2000 TSOP

     the entire flash media constituting the "socket", may be divided into up
     to four independent regions (referred to as "disks"). All such regions 
     ("disks") can be read/write accessed independently from other "disks" of 
     that "socket".

     "Sockets" consisting of either DiskOnChip 2000 DIP/DIMM or DiskOnChip
     Millennium (8MB) parts do not support this feature. For these types of
     "sockets", the term "disk" is identical to term "socket".

1.4. Each "disk" can be partitioned (with QNX 'fdisk' utility) into up to four
     disk partitions. Each such disk partition could be individually formatted 
     with QNX 'dinit' utility, and mounted with QNX 'mount' utility.

1.5. When working with DiskOnChip Millennium Plus, up to two "disks" can have
     individual hardware-level access permissions (read/write).


2. INSTALLING DISKONCHIP DRIVER
-------------------------------

2.1. Distribution of this version of DiskOnChip driver for QNX 6.2/x86 consists 
     of the archive diskonchip514p3-qnx6-x86.tar.gz, which contains the 
     following files:

         - README.TXT              This file

         - devb-doc                DiskOnChip driver for QNX 6.x/x86

         - dformat                 DiskOnChip formatter for QNX 6.x/x86
 
         - bios.diskonchip.build   Sample "build" file for creating QNX 6.x/x86
                                   bootimages for booting from DiskOnChip

         - include                 Directory containing DiskOnChip header files. 
                                   These header files must be #included by the  
                                   application in case when it accesses  
                                   DiskOnChip's advanced features (see Chapter 7).

         - manual "Extended Functions of the DiskOnChip Driver".

2.2. QNX Momentics 6.x could be installed on hosts running Windows, Solaris
     or QNX6 operating systems.

     If you have installed QNX Momentics 6.x on QNX6 host, proceed with
     Section 2.3.

     If you have installed QNX Momentics 6.x on Windows host, proceed with
     Section 2.4.

     If you have installed QNX Momentics 6.x on Solaris host, proceed with
     Section 2.5.

2.3. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on QNX6 host.

     If you have installed QNX Momentics 6.x on Windows host, skip this Section,
     and proceed with Section 2.4 instead.

     If you have installed QNX Momentics 6.x on Solaris host, skip this Section,
     and proceed with Section 2.5 instead.

     Before you begin, you must be logged in as superuser ('root'). 

2.3.1. Create 'diskonchip' sub-directory under directory /usr/local, and 
       change to this directory:

           # mkdir -p /usr/local/diskonchip
           # cd /usr/local/diskonchip

2.3.2. Copy archive diskonchip514p3-qnx6-x86.tar.gz to /usr/local/diskonchip 
       directory.

2.3.3. Unpack archive diskonchip514p3-qnx6-x86.tar.gz:

           # gunzip diskonchip514p3-qnx6-x86.tar.gz
           # tar -xvf diskonchip514p3-qnx6-x86.tar

2.3.4. Copy DiskOnChip driver 'devb-doc' and formatter 'dformat' to both /sbin 
       and /x86/sbin directories:

           # cp devb-doc /sbin
           # cp devb-doc /x86/sbin

           # cp dformat /sbin
           # cp dformat /x86/sbin

2.3.5. Set correct file permissions for 'devb-doc' and 'dformat':        

           # chmod 775 /sbin/devb-doc /x86/sbin/devb-doc
           # chmod 775 /sbin/dformat /x86/sbin/dformat

2.3.6. Copy "build" file 'bios.diskonchip.build' to both /boot/build and 
       /x86/boot/build directory:

           # cp bios.diskonchip.build /boot/build
           # cp bios.diskonchip.build /x86/boot/build

2.3.7. Create subdirectory 'diskonchip' under /usr/include, and copy all 
       DiskOnChip header files to /usr/include/diskonchip:

           # mkdir /usr/include/diskonchip
           # cp include/*.h /usr/include/diskonchip

2.3.8. Skip the rest of Chapter 2.

2.4. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on Windows host.

     If you have installed QNX Momentics 6.x on QNX6 host, skip this Section,
     and proceed with Section 2.3 instead.

     If you have installed QNX Momentics 6.x on Solaris host, skip this Section,
     and proceed with Section 2.5 instead.

2.4.1. Copy archive diskonchip514p3-qnx6-x86.tar.gz to temporary directory, and
       unpack it.

       ====> NOTE. You can use WinZip utility to open .tar.gz archive.

2.4.2. Copy DiskOnChip driver 'devb-doc' and formatter 'dformat' to 
       <QNXsdk>\target\qnx6\x86\sbin directory:

           # copy <tmp>\devb-doc <QNXsdk>\target\qnx6\x86\sbin
           # copy <tmp>\dformat  <QNXsdk>\target\qnx6\x86\sbin

2.4.3. Copy "build" file 'bios.diskonchip.build' to 
       <QNXsdk>\target\qnx6\x86\boot\build directory

           # copy <tmp>\bios.diskonchip.build <QNXsdk>\target\qnx6\x86\boot\build

2.4.4. Create subdirectory 'diskonchip' under <QNXsdk>\target\qnx6\usr\include, 
       and copy all DiskOnChip header files there:

           # mkdir <QNXsdk>\target\qnx6\usr\include\diskonchip
           # copy <tmp>\include\*.h \usr\include\diskonchip

2.4.5. Skip the rest of Chapter 2.

2.5. This Section contains instructions for the case when QNX Momentics 6.x is
     installed on Solaris host.

     If you have installed QNX Momentics 6.x on QNX 6.2 host, skip this Section,
     and proceed with Section 2.3 instead.

     If you have installed QNX Momentics 6.x on Windows host, skip this Section,
     and proceed with Section 2.4 instead.

2.5.1. Copy archive diskonchip514p3-qnx6-x86.tar.gz to temporary directory, and
       unpack it:

           # gunzip <tmp>/diskonchip514p3-qnx6-x86.tar.gz
           # tar -xvf <tmp>/diskonchip514p3-qnx6-x86.tar

2.5.2. Copy DiskOnChip driver 'devb-doc' and formatter 'dformat' to 
       <QNXsdk>/target/qnx6/x86/sbin directory:

           # cp <tmp>/devb-doc <QNXsdk>/target/qnx6/x86/sbin
           # cp <tmp>/dformat  <QNXsdk>/target/qnx6/x86/sbin

2.5.3. Copy "build" file 'bios.diskonchip.build' to 
       <QNXsdk>/target/qnx6/x86/boot/build directory

           # cp <tmp>/bios.diskonchip.build <QNXsdk>/target/qnx6/x86/boot/build

2.5.4. Create subdirectory 'diskonchip' under <QNXsdk>/target/qnx6/usr/include, 
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

     ===> NOTE. Some DiskOnChip parts (for example, DiskOnChip Millennium TSOP
                or DiskOnChip Millennium Plus parts) are shipped from the factory
                unformatted and without firmware. Before you install QNX 6.x on
                these DiskOnChip parts, it's good idea to try booting DOS from 
                these DiskOnChip parts on your QNX target system. Appendix B 
                explains how to do this.
     
3.2. Attach hard disk to QNX target system, install QNX 6.x to that hard disk,
     and boot QNX target system from that hard disk.

3.3. Copy DiskOnChip driver 'devb-doc' and formatter 'dformat' to directory 
     /sbin on the target system. Make sure that both 'devb-doc' and 'dformat' 
     has correct access permissions:

         # chmod 775 /sbin/devb-doc /sbin/dformat

3.4. If you will be plugging DiskOnChip into QNX target system using one of 
     M-Systems's DiskOnChip Evaluation boards, check Evaluation board's jumper
     settings, to find out which address DiskOnChip part(s) are mapped to. These
     jumper settings are usually listed on the back of Evaluation boards. By 
     default, most M-Systems' Evaluation boards map DiskOnChip to address 
     0xd0000 (in DOS notation, D000:0000).
    
3.5. Shutdown QNX target system, and plug DiskOnChip into it.

3.6. Boot QNX target system from the hard disk, and login as superuser ('root').

3.7. Start DiskOnChip driver:

          # devb-doc

     After a short while, you should see the message similar to the one below:

          DiskOnChip driver, ver. 5.1.4p3
          Path=0 - M-Systems DOC
          target=0 lun=0 Direct-Access(0) - M-Systems DOC

     At this point you may need to hit <ENTER> to get back to shell prompt.

     ===> NOTE. There are many configuration options that can be passed to 
                DiskOnChip driver via command line parameters. To see short 
                description of these options, execute:

                    # use devb-doc

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

         # devb-doc doc access=0x00000006
 
     Note that "0x00000006" can be abbreviated to simply "0x6":

         # devb-doc doc access=0x6
 
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

         # devb-doc doc access=0x07060724

     Note that since DiskOnChip "sockets" #1 and #3 have their bytes in <mode> 
     specified as 0x7 (which is driver's default setting for access mode), it
     is possible to specify these bytes as zero (which is interpreted as request
     to leave existing default access mode settings unchanged for these 
     "sockets"). In this case <mode> can be specified as 0x00060024, or simply
     as 0x60024:

         # devb-doc doc access=0x60024


5. DRIVER'S CONFIGURATION OPTIONS
---------------------------------

5.1. DiskOnChip driver features numerous command line option, which allows user
     to modify various aspects of driver's behavior. All these command line 
     options must be specified in driver's command line after 'doc' keyword,
     for example:

         # devb-doc doc addr=0xd0000

     Multiple options can be specified, in which case they should be separated
     by commas: 

         # devb-doc doc addr=0xd0000,chain=3

     DiskOnChip driver supports up to four DiskOnChip "sockets" (see Section 1.3
     for definition of term "socket"). It's possible to pass different values
     to "addr=<address>" and "passwd=<string>" options for different "sockets" 
     by specifying 'doc' keyword once for every "socket":
     
         # devb-doc  doc addr=0xd0000  doc addr=0xd4000,passwd=abcdefgh

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

         # devb-doc doc addr=0xd0000 

5.3. To facilitate troubleshooting of DiskOnChip problems, DiskOnChip driver 
     can be put in verbose mode by passing it 'verbose' option:

         # devb-doc doc verbose

     In this case DiskOnChip driver will issue messages at critical points of
     it's operations, and report all internal errors encountered when accessing
     DiskOnChip.

5.4. To configure DiskOnChip driver for various types of hardware interfaces to
     DiskOnChip, use 'access=<mode>' option. See Chapter 4 for more details
     regarding this option.

5.5. This Section applies to DiskOnChip Millennium Plus parts only. It does not
     apply to DiskOnChip 2000 and DiskOnChip Millennium parts.

     DiskOnChip Millennium Plus parts provide user with option to write- and/or 
     read- protect individual "disks" (see Section 1.4 for definition of term
     "disk"). To get access to such protected "disks", user has to specify
     8-character password via 'passwd=<string>' option. Here is example for the
     common case when there is only one "disk" on DiskOnChip Millennium Plus,
     protected by password "12345678":

         # devb-doc doc passwd=12345678

     Here is example for more complex case, when there are four "disks" on
     DiskOnChip Millennium Plus part, with second and third "disks" protected by
     passwords "12345678" and "abcdefgh" respectively, and first and forth disks
     unprotected:

         # devb-doc doc passwd,passwd=12345678,passwd=abcdefgh,passwd

5.6. By default, DiskOnChip driver allocates internal lookup tables in memory, 
     which significantly improves I/O performance. The combined size of all 
     these lookup tables can be roughly estimated as 0.1% of DiskOnChip's 
     capacity (for example, lookup tables for 32 MByte DiskOnChip part will
     take about 30 KBytes of RAM). While this is acceptable for many systems,
     in case of low end system with limited memory space, user can reduce 
     driver's memory footprint by using 'notab' option:

           # devb-doc doc notab 

      ===> NOTE. The 'notab' option will have substantial negative impact 
                 on DiskOnChip' performance.

5.7. By default, DiskOnChip driver favors I/O performance over I/O determinism.
     To configure driver to favor I/O determinism over I/O performance, use
     options 'chain=<number>' and 'asap'. Both of these options are described
     in detail in Chapter 9. 

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

         # devb-doc doc verify=0x3

     Here is more complex example for the system with four DiskOnChip "sockets".
     First and third "sockets" are configured for maximum verification; second
     "socket" configured for medium verification, and verification setting for
     forth "socket" is left unchanged. The <mode> parameter in this case is
     specified as 0x3B (which is 00111011 in binary notation): 

         # devb-doc doc verify=0x3B
 
      ===> NOTE. Setting verification mode to maximum value could negatively
                 impact DiskOnChip's 'write' performance.

5.9. DiskOnChip driver provides two options for setting verification mode for 
     'write' operations which driver performs internall when user issues devctl()
     calls to DiskOnChip (see Chapter 7). These options are "bverify=<mode>" and
     "vother=<mode>". In both cases, two least sigificant bits in <mode> 
     parameter specify verification mode, with '1' for minimal verification
     and '3' for maximum verification. These options are provided for completeness
     only; rarely there is a reason for changing driver's default settings for
     these options.

5.10. When starting, DiskOnChip driver, perform quick check of low-level flash
      format on the DiskOnChip. To instruct DiskOnChip driver to perform more
      thorough checking of low-level format during startup, use "chkdsk" option:
      
         # devb-doc doc chkdsk
       
      ===> NOTE. The "chkdsk" option could make driver's startup time longer.

5.11. By default, DiskOnChip driver moves data to/from DiskOnChip in 32-bit long
      words. While most of the boards can reliably translate 32-bit access
      cycles into correct sequence of 8-bit cycles, some boards can't do it 
      reliably. To make DiskOnChip driver working with these boards, use option
      '8bit'. This option instructs DiskOnChip driver to always move data to/from
      DiskOnChip in bytes rather then 32-bit long words:

         # devb-doc doc 8bit
  
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

         # devb-doc doc addr=0xc0000000,prio=9
  
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
 
         # devb-doc doc addr=0xc0000000,nosig=15

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

         # slay devb-doc

6.2. DiskOnChip is formatted with 'dformat' utility. In most cases, DiskOnChip 
     can be formatted by simply executing 'dformat' without any arguements:

         # dformat

     It is recommended to reboot system after DiskOnChip has been formatted.

6.3. By default, 'dformat' scans memory range [0xc8000..0xf0000] in host address
     space, probing for DiskOnChip. These memory probs can potentially interfere
     with other memory mapped devices (such as older network cards or SCSI 
     adapters). If address of DiskOnChip is known in advance, it makes sense to
     avoid memory scanning by telling DiskOnChip driver to look for DiskOnChip
     at this particular address only. The address can be passed to 'dformat' 
     via '-w:<address>' command line option. For example, if it's known in
     advance that DiskOnChip is installed at address 0xd0000:

         # dformat -w:d0000 

     ===> NOTE. Note that address is specified as 'd0000', not '0xd0000'. This
                is different from both 'devb-doc' driver (which expects address
                to be specified as "0xd0000", see Section 5.1) and M-Systems'
                DOS-base tools like DFORMAT.EXE (which expects it to be specified
                as DOS-style segment address "d000").

6.4. By default, 'dformat' accesses DiskOnChip in 32-bit mode. While most of
     x86 boards are capable of reliably translating 32-bit cycles into
     sequence of 8-bit cycles, there is a chance that some older boards won't
     be able to do so. In this case command line option '-8' must be used, 
     to force 'dformat' to access DiskOnChip strictly in 8-bit mode:

         # dformat -8
  
6.5. To facilitate troubleshooting of DiskOnChip problems, 'dformat' can be put
     in 'verbose' by specifying '-v' command line option:

         # dformat -v 

     In this case 'dformat' will issue messages at critical points of formatting
     operation, and report all internal errors it encounters (if any).

6.6. By default, 'dformat' uses the entire flash media (with exception of 
     approximately 150 KBytes at the beginning of the flash media which is 
     occupied by DiskOnChip's firmware) when formatting DiskOnChip. To instruct
     'dformat' to skip region of flash memory immediately following the firmware
     area, and start formatted data area at particular offset from firmware area, 
     use '-b' command line option. For example, to reserve 2 MBytes of flash 
     space, and start formatting at 2 MByte offset from firmware area:

         # dformat -b:2m

     In this case, the media layout will be as follows: there will be 150 KBytes
     at the beginning of the media that is occupied by DiskOnChip's firmware,
     then 2 MBytes of unformatted flash space; the the rest of the media will
     be formatted for file system storage.

     Here is another example of reserving 700 KBytes of flash space:
 
         # dformat -b:700k

6.7. This Section applies to DiskOnChip Millennium Plus and DiskOnChip 2000 TSOP
     parts only. It does not apply to other DiskOnChip 2000 parts and DiskOnChip
     Millennium parts.

     DiskOnChip Millennium Plus and DiskOnChip 2000 TSOP parts allow user to
     create multiple "disks" on single physical DiskOnChip part (see Section 1.4
     for definition of term "disk"). This can be achieved by using '-d' command 
     line option. For example, if you want to create three "disks" on 
     DiskOnChip Millennium Plus part (which is currently available in 32MByte
     capacity), with disk sizes of 5 MBytes, 10 MBytes and 16800 KBytes 
     respectively, you would execute:
     
         # dformat -d:5m,10m,16800k

6.8. This Section applies to DiskOnChip Millennium Plus parts only. It does not
     apply to DiskOnChip 2000 and DiskOnChip Millennium parts.

     DiskOnChip Millennium Plus parts provide user with option to write- and/or 
     read- protect individual "disks" (see Section 1.4 for definition of term
     "disk"). While current version of 'dformat' does not allow user to specify
     read- or write- protection for "disks" that it creates, it does allow user
     to turn off read- and write- protection on existing "disks" (probably
     created by M-Systems' DOS-based DFORMAT.EXE utility). In order to turn off
     read- or write- protection on every such "disk", user has to specify 
     8-character password via '-p' command line option. Here is example for 
     common case when there is only one read- or write- protected "disk" on 
     DiskOnChip Millennium Plus:

         # dformat -p:12345678

     where "12345678" is the password. 

     Here is example for more complex case, when there are four "disks" on
     DiskOnChip Millennium Plus part, with second and third "disks" protected
     by passwords "12345678" and "abcdefgh" respectively, and first and
     forth disks unprotected:

         # dformat -p:,12345678,abcdefgh,

6.9. To configure 'dformat' utility for various types of hardware interfaces to
     DiskOnChip (see Chapter 4), use '-a:<mode>' command line option. The <mode>
     parameter is specified in exactly the same fashion as in case of DiskOnChip
     driver's "access=<mode>" option (see Chapter 4). For example, in case when
     DiskOnChip's pin A0 is connected to line A0 of system's address bus; board 
     supports reading/writing of 32-bit long words to long-word-aligned addresses
     and 16-bit short words to short-word-aligned addresses within DiskOnChip
     memory windows, but does not allow access to individual bytes, you would 
     execute:

         # dformat -a:0x6
 

7. ACCESS TO DISKONCHIP'S ADVANCED FUNCTIONS
--------------------------------------------

     DiskOnChip driver provides user with full access to the following DiskOnChip
     Extended Functions:

         FL_IOCTL_BDK_OPERATION
         FL_IOCTL_BDTL_HW_PROTECTION
         FL_IOCTL_BINARY_HW_PROTECTION
         FL_IOCTL_CUSTOMER_ID
         FL_IOCTL_DEEP_POWER_DOWN_MODE
         FL_IOCTL_DEFRAGMENT
         FL_IOCTL_DELETE_SECTORS
         FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES
         FL_IOCTL_EXTENDED_WRITE_IPL
         FL_IOCTL_GET_INFO
         FL_IOCTL_INQUIRE_CAPABILITIES
         FL_IOCTL_MOUNT_VOLUME
         FL_IOCTL_NUMBER_OF_PARTITIONS
         FL_IOCTL_OTP
         FL_IOCTL_PLACE_EXB_BY_BUFFER
         FL_IOCTL_READ_SECTORS
         FL_IOCTL_UNIQUE_ID
         FL_IOCTL_VERIFY_VOLUME
         FL_IOCTL_WRITE_PROTECT
         FL_IOCTL_WRITE_SECTORS

     ===> NOTE. Complete description of all DiskOnChip Extended Functions, along
                with their associated data structures, can be found in M-Systems'
                manual "Extended Functions of the DiskOnChip Driver".
      
     All Extended Functions are accessed via devctl() calls to the DiskOnChip
     driver (which must be running at the moment when application issues devctl()
     call to it).

     The rest of this Chapter explain how to access DiskOnChip Extended Functions
     via devctl() calls to DiskOnChip driver. Appendix A contains complete code 
     example of accessing FL_IOCTL_GET_INFO DiskOnChip Extended Function via
     devctl() call to the DiskOnChip driver.

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
     it dynamically via malloc() rather then place it in stack:
     

         #include <malloc.h>
         #include <diskonchip/fldrvnto.h>

         flIoctlGeneric * req;

         req = malloc( sizeof(*req) );

         if (req == NULL) 
         {
             /* error */
         }
         

7.3. Consult "Extended Functions of the DiskOnChip Driver" manual (included as
     part of DiskOnChip driver distribution) to find out which input and output
     data structures must be passed to DiskOnChip driver when accessing 
     particular DiskOnChip Extended Function, and identify fields in 
     'flIoctlGeneric' data structure (defined in header diskonchip/fldrvnto.h)
     that corresponds to these input and output data structures. For example, 
     in case of FL_IOCTL_DEFRAGMENT Extended Function, you will need to pass 
     'flDefragInput' as input data structure and 'flDefragOutput' as output data
     structure, so you will use 'flIoctlGeneric.in.defragInput' and 
     'flIoctlGeneric.out.defragOutput' fileds.

7.4. Some of DiskOnChip Extended Function require user to pass data buffers along
     with input data structure. For example, when using FL_IOCTL_BDK_OPERATION 
     Extended Function, user must passed data buffer along with the input data 
     structure 'flIoctlGeneric.in.bdkOperationInput'. In all such cases, user 
     data must be copied into 'flIoctlGeneric.uData' input/output buffer. The 
     size of 'flIoctlGeneric.uData' input buffer is currently 2 KBytes.

     ===> NOTE. Contents of 'flIoctlGeneric.uData' input buffer are not 
                preserved during devctl() call.
     
7.5. To access particular DiskOnChip's Extended Function, issue devctl() call to
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


7.6. If particular DiskOnChip Extended Function returns status code, you should 
     check it. For example, FL_IOCTL_GET_INFO Extended Function returns status 
     code in field 'flIoctlGeneric.out.diskInfoOutput.status':


         #include "diskonchip/fldrvnto.h"

         if (req.out.diskInfoOutput.status != flOK) 
         {
             /* error, see error codes in diskonchip/flstatus.h */
         }

7.7. Some of DiskOnChip Extended Functions (for example, FL_IOCTL_BDK_OPERATION), 
     in addition to returning output data structure in the respective field in 
     structure 'flIoctlGeneric', also return up to 2 KBytes of data. In all such
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

         # devb-doc blk commit=high
        
     All the options following "blk" keyword will be passed along to QNX disk
     I/O manager 'io-blk'. These io-blk parameters can be combined with DiskOnChip
     options (following "doc" keyword), as shown below:

         # devb-doc doc addr=0xd0000,verbose blk commit=high

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
     of 'write' operations. Sections 8.2-8.3 describe driver's configuration
     options that can be used for this purpose. 

9.2. Default configuration of DiskOnChip driver allows for up to 20 flash blocks
     to be erased during single sector 'write' operation. With average duration
     of flash erase operation on DiskOnChip to be within few milliseconds (for 
     example, in case of DiskOnChip Millennium Plus parts, the average duration
     of flash erase operation is 2 milliseconds), single sector 'write' operation
     (which is normally completed within 1 millisecond) could occasionally take 
     up to 40 milliseconds. If such fluctuation in duration of sector 'write'
     operations is unacceptable for particular system, user could reduce it
     lowering the limit on number of flash blocks that can be erased during 
     single sector 'write' operation. This limit can be sepecified via driver's
     'chain' command line option. For example, to make sure that every sector 
     'write' operation is completed within 10 milliseconds, user should start 
     DiskOnChip driver as shown below:

         # devb-doc doc chain=4

     The minimal value for 'chain' option is 2, maximum is 30.
 
     ===> NOTE. This option can cause decrease in DiskOnChip's 'write' 
                performance. In extreme case, when 'chain' parameter is set to
                minimal value of 2, 'write' performance sometimes could be as
                low as half of the normal one.

9.3. This Section is specific to DiskOnChip Millennium Plus, Mobile DiskOnChip, 
     and DiskOnChip 2000 TSOP parts. If you are using DiskOnChip parts other
     the listed above, skip this Section.

     By default, DiskOnChip driver uses flash management algorithms that favor
     I/O performance over I/O determinism. To instruct DiskOnChip driver to
     favor I/O determinism over I/O performance, and make 'write' operations on
     DiskOnChip more deterministic, use driver's 'asap' command line option.
     Here is an example:

         # devb-doc doc asap


10. KNOWN LIMITATIONS
---------------------

10.1. If hardware write protection is activated on one of DiskOnChip "disks" (see
      Section 1.4 for definition of term "disk"), but this disk was not mounted as
      write protected, then attempt to write data to this disk will not produce 
      immediate error message from DiskOnChip driver. This is cause by the fact
      that data is usually held in disk cache for a while, and written to the disk
      with some delay.
 
10.2. Read- and write-protected "disk" on DiskOnChip must not use string 
      "00000000" as password.

10.3. While current version of 'dformat' allows user to create multiple "disks"
      on physical DiskOnChip Millennium Plus or DiskOnChip 2000 TSOP part, it 
      does not allow user to specify write- and read- protection for these 
      "disks" - all "disks" are created writeable and readable. If you want to
      write- or/and read- protect these "disks", use M-Systems' DOS-based 
      DFORMAT.EXE utility for this purpose. This utility is part of M-Systems'
      utility set for DOS; it can be downloaded from M-Systems' web site 
      http://www.m-sys.com.


11. TROUBLESHOOTING
-------------------

11.1. If you are experiencing problems with DiskOnChip, run DiskOnChip driver 
      with 'verbose' option (see Section 5.3). This will cause driver to issue
      error messages if it encounteres errors, as well as number of diagnostic
      messages during driver's startup.

11.2. DiskOnChip driver cannot detect DiskOnChip. 

      Possible solutions:

         - Check if DiskOnChip is properly inserted in it's socket. The blue dot 
           found on the sticker on DiskOnChip 2000, or cut side of DiskOnChip
           Millennium, should be on the cut side of DIP socket.

         - Check BIOS setup, to verify that there is no memory shadowing in 
           the memory region that DiskOnChip is mapped to.

         - There is memory conflict with other memory-mapped device using the 
           same memory range (for example, older network adapters or SCSI 
           cards). Try installing DiskOnChip at different address, or removing
           other memory-mapped cards.

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

        M-Systems Inc.8371 Central Ave, Suite A Newark CA  94560
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
     *  Obtain DiskOnChip information using FL_IOCTL_GET_INFO Extended Function.
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
    
        /* check status code of DiskOnChip Extended Function */
    
        if (req->out.diskInfoOutput.status != flOK) 
            return -1;      /* error in Extended Function */
    
        /* display info received from DiskOnChip Extended Function */
    
        info = &req->out.diskInfoOutput.info;
    
        printf ("DiskOnChip info\n");
        printf ("\twindow linear address                 : 0x%lx\n",       info->baseAddress);
        printf ("\tcontroller type                       : 0x%x ",   (int) info->DOCType);

        switch( (int) info->DOCType ) 
        {

            case FL_DOC:         printf( "(DiskOnChip 2000 DIP)\n" );        break; 
            case FL_MDOC:        printf( "(DiskOnChip Millennium)\n" );      break; 
            case FL_DOC2000TSOP: printf( "(DiskOnChip 2000 TSOP)\n" );       break; 
            case FL_DOC2400:     printf( "(DiskOnChip 2400)\n" );            break; 
            case FL_MDOCP:       printf( "(DiskOnChip Millennium Plus)\n" ); break; 
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

B.1. Download the latest version of DiskOnChip utility set for DOS from 
     M-Systems' web site http://www.m-sys.com (you will need version 5.x.x
     of these utilities).

B.2. Prepare DOS boot floppy. 

B.3. Copy M-Systems DFORMAT.EXE utility and DiskOnChip firmware file to DOS 
     floppy.

     ===> NOTE. DiskOnChip firmware file is part of M-Systems utility set. It 
                usually has the filename DOC<ver>.EXB, where <ver> is the version
                of utility set (for example, DOC514.EXB).

B.4. Boot QNX target system with DOS from floppy.

B.5. Re-format DiskOnChip using M-Systems' DFORMAT.EXE utility, as shown below:

         DFORMAT /WIN=<address> /S=<firmware> /Y

     where:

         <address>   -  base address of DiskOnChip (i.e. d000, d800 etc.). Note
                        that it's "d000" (4-digit), not "d0000" nor "0xd000". 

         <firmware>  -  DiskOnChip firmware file (for example, DOC514.EXB).

     For instance, if DiskOnChip is mapped to address d000:0000, and you are 
     using DOC514.EXB firmware file, you execute the following command:

         DFORMAT /WIN=d000 /S=DOC514.EXB /Y

B.6. Cycle power on QNX target system, to allow DiskOnChip firmware to install
     itself.

B.7. Find out which DOS drive letter the DiskOnChip has installed itself at.
     Assuming that there is no any DOS/Windows partitions on system's hard disk,
     you should see DiskOnChip appearing at driver letter C:.

B.8. Assuming DiskOnChip has installed itself at driver letter C:, execute
     the following command:

         SYS C:

     to transfer DOS boot files to DiskOnChip.

B.9. Remove DOS floppy from the drive, and reboot system. Enter BIOS setup, and
     disable all hard disks. You should see DOS booting from the DiskOnChip.

B.10. Undo the changes that were made in BIOS setup in Section B.9.



