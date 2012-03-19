Building busybox with the standard android NDK
==============================================

The aim is to gather information and patches on how to build busybox using the compiler shipped with the android NDK.

I recently discovered that a number [[1](http://lists.busybox.net/pipermail/busybox/2012-March/077486.html),[2](http://lists.busybox.net/pipermail/busybox/2012-March/077505.html)] of upstream changes make it possible to build the latest git version of busybox, __without requiring any patches__:

    # get busybox sources
    git clone git://busybox.net/busybox.git
    # use default upstream config
    cp configs/android_ndk_defconfig .config
    
    # add arm-linux-androideabi-* to your PATH
    export PATH="$PATH:/path/to/your/android-ndk/android-ndk-r7b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/"
    # if android-ndk is not installed in /opt/android-ndk, edit SYSROOT= in .config
    xdg-open .config
    
    # build it!
    make

However, a fair number of options are disabled in that config, because they give an error when building. See below for an overview.

Bigger config
-------------
* File *android\_ndk\_defconfigPlus* contains all the options of upstream configs/android\_ndk\_defconfig PLUS the options of 'make defconfig' that can be built.


Config options that do not build, code error
--------------------------------------------
These errors indicate bugs (usually in the restricted android libc library, called bionic), and can often be fixed by adding patches to the busybox code.

* All of *Login/Password Management Utilities*  --  error: 'struct passwd' has no member named 'pw\_gecos'
 * disables CONFIG\_ADD\_SHELL, CONFIG\_REMOVE\_SHELL, CONFIG\_ADDUSER, CONFIG\_ADDGROUP, CONFIG\_DELUSER, CONFIG\_DELGROUP, CONFIG\_GETTY, CONFIG\_LOGIN, CONFIG\_PASSWD, CONFIG\_CRYPTPW, CONFIG\_CHPASSWD, CONFIG\_SU, CONFIG\_SULOGIN, CONFIG\_VLOCK
* CONFIG\_ARPING  --  networking/arping.c:96: error: invalid use of undefined type 'struct arphdr'
* CONFIG\_BRCTL  --  networking/brctl.c:70: error: conflicting types for 'strtotimeval'
* CONFIG\_ETHER\_WAKE  --  networking/ether-wake.c:275: error: 'ETH_ALEN' undeclared (first use in this function)
* CONFIG\_FEATURE\_IPV6  --  networking/ifconfig.c:82: error: redefinition of 'struct in6\_ifreq'
 * disables CONFIG\_PING6, CONFIG\_FEATURE\_IFUPDOWN\_IPV6, CONFIG\_TRACEROUTE6
* CONFIG\_FEATURE\_UTMP, CONFIG\_FEATURE\_WTMP  --  init/halt.c:86: error: 'RUN_LVL' undeclared (first use in this function)
 * disables CONFIG\_WHO, CONFIG\_USERS, CONFIG\_LAST, CONFIG\_RUNLEVEL, CONFIG\_WALL
* CONFIG\_FSCK\_MINIX, CONFIG\_MKFS\_MINIX  --  util-linux/fsck\_minix.c:111: error: 'INODE\_SIZE1' undeclared here (not in a function)
* CONFIG\_INETD  --  /opt/android-ndk/platforms/android-9/arch-arm/usr/include/linux/un.h:18: error: expected specifier-qualifier-list before 'sa\_family\_t' and networking/inetd.c:562: error: 'struct sockaddr\_un' has no member named 'sun\_path'
* CONFIG\_IONICE  --  **has patch** -- miscutils/ionice.c:23: error: 'SYS\_ioprio\_set' undeclared (first use in this function)
* CONFIG\_LFS  --  include/libbb.h:256: error: size of array 'BUG\_off\_t\_size\_is\_misdetected' is negative
* CONFIG\_LOGGER  --  sysklogd/logger.c:36: error: expected ';', ',' or ')' before '*' token
* CONFIG\_NSLOOKUP  --  networking/nslookup.c:126: error: dereferencing pointer to incomplete type
* CONFIG\_SWAPONOFF  --  util-linux/swaponoff.c:96: error: 'MNTOPT\_NOAUTO' undeclared (first use in this function)
* CONFIG\_ZCIP  --  networking/zcip.c:51: error: field 'arp' has incomplete type

Config options that do not build, missing library
-------------------------------------------------
These errors indicate that the library is missing from androids libc implementation

* sys/sem.h
 * CONFIG\_IPCS  --  util-linux/ipcs.c:32:21: error: sys/sem.h: No such file or directory
 * CONFIG\_LOGREAD  --  sysklogd/logread.c:20:21: error: sys/sem.h: No such file or directory
 * CONFIG\_SYSLOGD  --  sysklogd/syslogd.c:68:21: error: sys/sem.h: No such file or directory

* sys/kd.h
 * CONFIG\_CONSPY  --  miscutils/conspy.c:45:20: error: sys/kd.h: No such file or directory
 * CONFIG\_LOADFONT, CONFIG\_SETFONT  --  console-tools/loadfont.c:33:20: error: sys/kd.h: No such file or directory

* others
 * CONFIG\_EJECT  --  miscutils/eject.c:30:21: error: scsi/sg.h: No such file or directory
 * CONFIG\_FEATURE\_HAVE\_RPC, CONFIG\_FEATURE\_INETD\_RPC  --  networking/inetd.c:176:22: error: rpc/rpc.h: No such file or directory
 * CONFIG\_FEATURE\_IFCONFIG\_SLIP  --  networking/ifconfig.c:59:26: error: net/if\_slip.h: No such file or directory
 * CONFIG\_FEATURE\_SHADOWPASSWDS, CONFIG\_USE\_BB\_SHADOW  --  include/libbb.h:61:22: error: shadow.h: No such file or directory
 * CONFIG\_HUSH  --  shell/hush.c:89:18: error: glob.h: No such file or directory
 * CONFIG\_IFENSLAVE  --  networking/ifenslave.c:132:30: error: linux/if\_bonding.h: No such file or directory
 * CONFIG\_IFPLUGD  --  networking/ifplugd.c:38:23: error: linux/mii.h: No such file or directory
 * CONFIG\_IPCRM  --  util-linux/ipcrm.c:25:21: error: sys/shm.h: No such file or directory
 * CONFIG\_MT  --  miscutils/mt.c:19:22: error: sys/mtio.h: No such file or directory
 * CONFIG\_NTPD  --  networking/ntpd.c:49:23: error: sys/timex.h: No such file or directory
 * CONFIG\_SETARCH  --  util-linux/setarch.c:23:29: error: sys/personality.h: No such file or directory
 * CONFIG\_WATCHDOG  --  miscutils/watchdog.c:24:28: error: linux/watchdog.h: No such file or directory
 * CONFIG\_UBI* -- miscutils/ubi\_tools.c:67:26: error: mtd/ubi-user.h: No such file or directory
  * disables CONFIG\_UBIATTACH, CONFIG\_UBIDETACH, CONFIG\_UBIMKVOL, CONFIG\_UBIRMVOL, CONFIG\_UBIRSVOL, CONFIG\_UBIUPDATEVOL

Config options that give a linking error
----------------------------------------
Androids libc implementation claims to implement the methods in the error, but surprisingly does not.

* mntent -- **has patch**
 * CONFIG\_DF  --  undefined reference to 'setmntent', 'endmntent'
 * CONFIG\_FSCK  --  undefined reference to 'setmntent', 'getmntent\_r', 'endmntent'
 * CONFIG\_MKFS\_EXT2  --  undefined reference to 'setmntent', 'endmntent'
 * CONFIG\_MOUNTPOINT  --  undefined reference to 'setmntent', 'endmntent'
 * CONFIG\_MOUNT  --  undefined reference to 'setmntent', 'getmntent\_r'
 * CONFIG\_UMOUNT  --  undefined reference to 'setmntent', 'getmntent\_r', 'endmntent'

* getsid
 * CONFIG\_KILL  --  undefined reference to 'getsid'
 * CONFIG\_KILLALL  --  undefined reference to 'getsid'
 * CONFIG\_KILLALL5  --  undefined reference to 'getsid'
 * CONFIG\_PGREP  --  undefined reference to 'getsid'
 * CONFIG\_PKILL  --  undefined reference to 'getsid'

* stime
 * CONFIG\_DATE  --  **has patch** -- undefined reference to 'stime'
 * CONFIG\_RDATE  --  undefined reference to 'stime'

* others
 * CONFIG\_ADJTIMEX  --  undefined reference to 'adjtimex'
 * CONFIG\_FEATURE\_HTTPD\_AUTH\_MD5  --  undefined reference to 'crypt'
 * CONFIG\_HOSTID  --  undefined reference to 'gethostid'
 * CONFIG\_HOSTNAME  --  undefined reference to 'sethostname'
 * CONFIG\_LOGNAME  --  undefined reference to 'getlogin\_r'
 * CONFIG\_MICROCOM  --  undefined reference to 'cfsetspeed'
 * CONFIG\_NAMEIF  --  undefined reference to 'ether\_aton\_r'
 * CONFIG\_PIVOT\_ROOT  --  undefined reference to 'pivot\_root'
 * CONFIG\_STAT  --  **has patch** -- undefined reference to 'S\_TYPEISMQ', 'S\_TYPEISSEM', 'S\_TYPEISSHM'
 * CONFIG\_UDHCPD  --  undefined reference to 'ether\_aton\_r'