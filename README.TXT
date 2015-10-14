# ************************************************************************
# **  MICROCHIP
# **   This took quite a bit of time and effort on my part, and I'm broke.
# **   PLEASE send me some swag...
# **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome
# ************************************************************************

Please note that a tremendous amount of time went into making this code
usable by--and bringing these bugs to the attention of--your staff.
Otherwise, all this effort serves little benefit to my purposes, as
I'd already figured out the -f<options> workaround quite a while ago and
have been using them since I discovered this bug.

e.g. (Things that were not necessary for my own purposes):
* location of the actual bugs' appearance, likely due to constant-usage
  (rather than assigning registers)
* The conditions necessary (like rand()) to make them appear
* Location/identification of the printf() / PRIi8/PRId8 bug
* submission of a ticket
* using stdio

Note also: The math-bug could present serious threats to safety... 
E.G. This was discovered in a motor-driving application, using source-code
 that has run on several other architectures without fail. 
 Where 'power' should've been -127, it was instead 32385!
 What if this were a drop-in replacement for a large CNC machine?
 What if this was in a pace-maker...?

Recommendation-letters/job-application forms (and/or swag) can be sent to:
   <removed for upload-purposes>

--------------------------------------------------------------------------

TO SEE THE ERROR:
   enter ./errorsOnly/ and run 'make clean' then 'make'
   This is for a PIC32MX170F256B
   Upload (to flash) as per your hardware.
   UART1 on RB15 is used for output at 9600bps

   ALL PRINTOUT should be "127, -1, -127"
   In ALL cases:
    power = 127
    dir = -1
    signedPower = power * dir   
   Instead: 
    dir is treated as a large integer with value 0x000000ff
    BOTH in printout AND usage.

    CASE 1: shows good math/printout
    CASE 2: shows bad printout, but good math
    CASE 3: shows BAD MATH.
     This case is not just a printout error,
     The bad-math is *used* and resulted in WILD motor-behavior.


FOR OBSERVATIONS, EXPLANATIONS, AND EXPERIMENTS:
   do the following from the main directory (this one)


To compile for a PIC32MX170F256B:
   'make clean'
   'make'
   You'll have to upload per your tools...
   (NOTE: This experiment was executed from FLASH)

To compile for a desktop-test application:
   'make clean'
   'make DESKTOP=1'
   'make run DESKTOP=1'
   ('CTRL-C' to exit)



BRIEF EXPLANATION:
   (see main.c for more detail)
   xc32-gcc WITH OPTIMIZATION
      copiles with MATH and PRINTOUT bugs when using int8_t 
      (and possibly other small integer types)

   This experiment was run on xc32-gcc v1.40 
      ("free XC32 C compiler")
      (for linux 32-bit, on a x86-32bit machine)


   IN errorsOnly/
      'main.c' and 'makefile' are simplified to the bare-necessities
          for recreating the bug
   IN the main directory:   
      'main.c' and 'makefile' contain many options and explanations
          for determining where the bug is located.
      DEFAULT OPTIONS cause the bug.

OPTIMIZATION seems to cause the bugs
   This can be experimented-with by choosing the appropriate OPT_ARG
   in 'makefile'

   ALSO: 'make ALL_FOPTS=1'
      Includes all the normal optimizations, explicitly invoked
      THIS DOES NOT CAUSE THE BUG.




'xc32-gcc -v' reports:

Microchip Language Tool Shell Version v1.40 (Build date: Jun  3 2015).
Copyright (c) 2012 Microchip Technology Inc. All rights reserved
*** Executing: "/opt/microchip/xc32/v1.40/bin/bin/xc32-gcc" "-v" 
Using built-in specs.
COLLECT_GCC=/opt/microchip/xc32/v1.40/bin/bin/xc32-gcc
COLLECT_LTO_WRAPPER=/opt/microchip/xc32/v1.40/bin/bin/gcc/pic32mx/4.8.3/lto-wrapper
Target: pic32mx
Configured with:
/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/src48x/gcc/configure
--build=i386-apple-darwin10 --host=i386-linux
--prefix=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/Linux32-image
--includedir=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/linux32-build/pic32mx/include
--bindir=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/Linux32-image/bin/bin
--libexecdir=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/Linux32-image/bin/bin
--with-libelf=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/linux32-build/linux-libs
--without-cloog --without-isl
--with-gmp=/Volumes/home/c11067/xc32/build/xc32-XC32_v1_40_branch/linux32-build/linux-libs
--target=pic32mx --program-prefix=xc32-
--with-bugurl=http://www.microchip.com/support --with-dwarf2 --with-newlib
--with-gnu-as --with-gnu-ld --enable-cxx-flags=-mno-smart-io --enable-lto
--enable-fixed-point --enable-gofast --enable-languages=c,c++
--enable-sgxx-sde-multilibs --enable-sjlj-exceptions --enable-obsolete
--disable-hosted-libstdcxx --disable-libstdcxx-pch --disable-libssp
--disable-libmudflap --disable-libffi --disable-libfortran
--disable-bootstrap --disable-shared --disable-__cxa_atexit --disable-nls
--disable-libgomp --disable-threads --disable-sim --disable-decimal-float
--disable-libquadmath --without-headers 'XGCC_FLAGS_FOR_TARGET=-frtti
-fexceptions -fno-enforce-eh-specs -mno-smart-io' --disable-checking
Thread model: single
gcc version 4.8.3 MPLAB XC32 Compiler v1.40 (Microchip Technology) 



This bug is not reproduceable in 'gcc' for the DESKTOP application example:
'gcc -v' reports:
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/i586-linux-gnu/4.9/lto-wrapper
Target: i586-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Debian 4.9.2-10'
--with-bugurl=file:///usr/share/doc/gcc-4.9/README.Bugs
--enable-languages=c,c++,java,go,d,fortran,objc,obj-c++ --prefix=/usr
--program-suffix=-4.9 --enable-shared --enable-linker-build-id
--libexecdir=/usr/lib --without-included-gettext --enable-threads=posix
--with-gxx-include-dir=/usr/include/c++/4.9 --libdir=/usr/lib --enable-nls
--with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug
--enable-libstdcxx-time=yes --enable-gnu-unique-object
--disable-vtable-verify --enable-plugin --with-system-zlib
--disable-browser-plugin --enable-java-awt=gtk --enable-gtk-cairo
--with-java-home=/usr/lib/jvm/java-1.5.0-gcj-4.9-i386/jre
--enable-java-home --with-jvm-root-dir=/usr/lib/jvm/java-1.5.0-gcj-4.9-i386
--with-jvm-jar-dir=/usr/lib/jvm-exports/java-1.5.0-gcj-4.9-i386
--with-arch-directory=i386 --with-ecj-jar=/usr/share/java/eclipse-ecj.jar
--enable-objc-gc --enable-targets=all --enable-multiarch
--with-arch-32=i586 --with-multilib-list=m32,m64,mx32 --enable-multilib
--with-tune=generic --enable-checking=release --build=i586-linux-gnu
--host=i586-linux-gnu --target=i586-linux-gnu
Thread model: posix
gcc version 4.9.2 (Debian 4.9.2-10) 

