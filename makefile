#/* mehPL:
# *    This is Open Source, but NOT GPL. I call it mehPL.
# *    I'm not too fond of long licenses at the top of the file.
# *    Please see the bottom.
# *    Enjoy!
# ************************************************************************
# **  MICROCHIP
# **   This took quite a bit of time and effort on my part, and I'm broke.
# **   PLEASE send me some swag...
# **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome
# ************************************************************************
# */
#
#
#




# NO BUG:
#OPT_ARG undefined

# NO BUG:
#OPT_ARG = -O0


# BUG:
#OPT_ARG = -O
# BUG:
OPT_ARG = -O1




# 'man gcc':
# -O and -O1 are the same...
#   -O turns on the following optimization flags:

#   -fauto-inc-dec -fcprop-registers -fdce -fdefer-pop -fdelayed-branch
#   -fdse -fguess-branch-probability -fif-conversion2 -fif-conversion
#   -finline-small-functions -fipa-pure-const -fipa-reference
#   -fmerge-constants -fsplit-wide-types -ftree-builtin-call-dce
#   -ftree-ccp -ftree-ch -ftree-copyrename -ftree-dce
#   -ftree-dominator-opts -ftree-dse -ftree-fre -ftree-sra -ftree-ter
#   -funit-at-a-time

#   -O also turns on -fomit-frame-pointer on machines where doing so
#   does not interfere with debugging.


# If called with e.g. 'make ALL_FOPTS=1'
#  OPT_ARG will be undefined
#   (No -O# argument will be evoked)
#  And all the following -f<optimizations> will be included instead
#  These optimizations are reported by:
#   'xc32-gcc -Q -O1 --help=optimizers'
#  And do NOT cause the bug
#  Thus, obviously, wherever the bug exists must be in an optimization
#   not be reported by 'xc32-gcc -Q -O1 --help=optimizers'
ifdef ALL_FOPTS
# -fpack-struct=<number>????
# -fpack-struct (without a number) is disabled...

CFLAGS += -faggressive-loop-optimizations
CFLAGS += -fbranch-count-reg
CFLAGS += -fcombine-stack-adjustments
CFLAGS += -fcommon
CFLAGS += -fcompare-elim
CFLAGS += -fcprop-registers
CFLAGS += -fdce
CFLAGS += -fdefer-pop
CFLAGS += -fdelayed-branch
CFLAGS += -fdelete-null-pointer-checks
CFLAGS += -fdse
CFLAGS += -fearly-inlining
CFLAGS += -fforward-propagate
CFLAGS += -fgcse-lm
CFLAGS += -fguess-branch-probability
CFLAGS += -fif-conversion
CFLAGS += -fif-conversion2
CFLAGS += -finline
CFLAGS += -finline-atomics
CFLAGS += -finline-functions-called-once
CFLAGS += -fipa-profile
CFLAGS += -fipa-pure-const
CFLAGS += -fipa-reference
CFLAGS += -fira-hoist-pressure
CFLAGS += -fivopts
CFLAGS += -fjump-tables
CFLAGS += -fmath-errno
CFLAGS += -fmerge-constants
CFLAGS += -fmove-loop-invariants
CFLAGS += -fomit-frame-pointer
CFLAGS += -fpeephole
CFLAGS += -fprefetch-loop-arrays
CFLAGS += -frename-registers

# C++ Only, but left for completeness:
CFLAGS += -frtti

CFLAGS += -fsched-critical-path-heuristic
CFLAGS += -fsched-dep-count-heuristic
CFLAGS += -fsched-group-heuristic
CFLAGS += -fsched-interblock
CFLAGS += -fsched-last-insn-heuristic
CFLAGS += -fsched-rank-heuristic
CFLAGS += -fsched-spec
CFLAGS += -fsched-spec-insn-heuristic
CFLAGS += -fsched-stalled-insns-dep
CFLAGS += -fshort-double
CFLAGS += -fshort-enums
CFLAGS += -fshrink-wrap
CFLAGS += -fsigned-zeros
CFLAGS += -fsplit-ivs-in-unroller
CFLAGS += -fsplit-wide-types

# C++ Only, but left for completeness:
CFLAGS += -fno-threadsafe-statics

CFLAGS += -ftrapping-math
CFLAGS += -ftree-bit-ccp
CFLAGS += -ftree-ccp
CFLAGS += -ftree-ch
CFLAGS += -ftree-coalesce-vars
CFLAGS += -ftree-copy-prop
CFLAGS += -ftree-copyrename
CFLAGS += -ftree-cselim
CFLAGS += -ftree-dce
CFLAGS += -ftree-dominator-opts
CFLAGS += -ftree-dse
CFLAGS += -ftree-forwprop
CFLAGS += -ftree-fre
CFLAGS += -ftree-loop-if-convert
CFLAGS += -ftree-loop-im
CFLAGS += -ftree-loop-ivcanon
CFLAGS += -ftree-loop-optimize
CFLAGS += -ftree-phiprop
CFLAGS += -ftree-pta
CFLAGS += -ftree-reassoc
CFLAGS += -ftree-scev-cprop
CFLAGS += -ftree-sink
CFLAGS += -ftree-slp-vectorize
CFLAGS += -ftree-slsr
CFLAGS += -ftree-sra
CFLAGS += -ftree-ter
CFLAGS += -ftree-vect-loop-version
CFLAGS += -funit-at-a-time

# "variable tracking requested, but useless unless producing debug info"
# But, again, left for completeness:
CFLAGS += -fvar-tracking
CFLAGS += -fvar-tracking-assignments

CFLAGS += -fweb



undefine OPT_ARG
endif




ifdef OPT_ARG
CFLAGS += $(OPT_ARG)
endif




CFLAGS += -funsigned-char -Wall



ifndef DESKTOP


CFLAGS += -mprocessor=32MX170F256B

all:
	xc32-gcc -c $(CFLAGS) -o main.o main.c 
	xc32-gcc $(CFLAGS) main.o --output main.elf -Wl,-Map=main.map,--cref 
	xc32-objcopy -O ihex main.elf main.hex2
	xc32-bin2hex main.elf 

run: 
	~/_commonCode/_make/rC3-scripts/pic32_flash3.sh \
	main.hex 32MX170F256B
else
all:
	gcc $(CFLAGS) -DDESKTOP_TEST -o main main.c
run:
	./main
endif

clean:
	rm -f main main.map main.elf main.hex main.hex2 main.o *.d




#/* mehPL:
# *    I would love to believe in a world where licensing shouldn't be
# *    necessary; where people would respect others' work and wishes, 
# *    and give credit where it's due. 
# *    A world where those who find people's work useful would at least 
# *    send positive vibes--if not an email.
# *    A world where we wouldn't have to think about the potential
# *    legal-loopholes that others may take advantage of.
# *
# *    Until that world exists:
# *
# *    This software and associated hardware design is free to use,
# *    modify, and even redistribute, etc. with only a few exceptions
# *    I've thought-up as-yet (this list may be appended-to, hopefully it
# *    doesn't have to be):
# * 
# *    1) Please do not change/remove this licensing info.
# *    2) Please do not change/remove others' credit/licensing/copyright 
# *         info, where noted. 
# *    3) If you find yourself profiting from my work, please send me a
# *         beer, a trinket, or cash is always handy as well.
# *         (Please be considerate. E.G. if you've reposted my work on a
# *          revenue-making (ad-based) website, please think of the
# *          years and years of hard work that went into this!)
# *    4) If you *intend* to profit from my work, you must get my
# *         permission, first. 
# *    5) No permission is given for my work to be used in Military, NSA,
# *         or other creepy-ass purposes. No exceptions. And if there's 
# *         any question in your mind as to whether your project qualifies
# *         under this category, you must get my explicit permission.
# *
# *    The open-sourced project this originated from is ~98% the work of
# *    the original author, except where otherwise noted.
# *    That includes the "commonCode" and makefiles.
# *    Thanks, of course, should be given to those who worked on the tools
# *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
# *    I'm certain many others. 
# *    And, as well, to the countless coders who've taken time to post
# *    solutions to issues I couldn't solve, all over the internets.
# *
# *
# *    I'd love to hear of how this is being used, suggestions for
# *    improvements, etc!
# *         
# *    The creator of the original code and original hardware can be
# *    contacted at:
# *
# *        EricWazHung At Gmail Dotcom
# *
# *    The site associated with the original open-sourced project is at:
# *
# *        https://sites.google.com/site/geekattempts/
# *
# * This license added to the original file located at:
# * /home/meh/_pic32Projects/printfTest/6-revisit_nochange/makefile
# *
# *    (Wow, that's a lot longer than I'd hoped).
# *
# *    Enjoy!
# ************************************************************************
# **  MICROCHIP
# **   This took quite a bit of time and effort on my part, and I'm broke.
# **   PLEASE send me some swag...
# **   Or, yahknow, a remote/telecommuting/contracting gig would be awesome
# ************************************************************************
# */
