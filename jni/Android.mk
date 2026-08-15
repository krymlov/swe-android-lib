LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := swe-2.10.03j6f

# -O2 is here for one reason: ndk-build defaults LOCAL_ARM_MODE to thumb, and its thumb release
# preset is "-mthumb -Oz -DNDEBUG" (build/core/toolchains/arm-linux-androideabi-clang/setup.mk),
# while arm64-v8a, x86 and x86_64 all get plain -O2. So armeabi-v7a alone was being built with
# clang's *minimum size* mode - more aggressive about size than -Os, and a poor trade for a
# library that is almost entirely double-precision arithmetic. Nobody chose that; it fell out of
# the thumb default. LOCAL_CFLAGS land after the NDK's own flags on the command line, so this
# -O2 wins for armeabi-v7a and is a no-op for the other three, which already had it.
#
# Costs 11% on the armeabi-v7a .so (478 KB -> 532 KB) and nothing on the others. Kept in thumb
# mode deliberately: -marm -O2 comes out at 595 KB (+24%), and thumb-2 on ARMv7 gives up very
# little speed for that.
#
# Not raised to -O3: that grows every ABI (+5% on arm64/x86_64, +17% on armeabi-v7a) and there
# is no ARM hardware or emulator image here to measure the benefit on, so it would be a size
# cost taken on faith. Re-open it with a real device; see
# ai-github-projects/swe-android-lib/tools/opt-experiment-so.sh for the harness.
#
# Deliberately NOT copied over from the desktop build: -fno-math-errno. It is worth 3.6% there,
# but on Android it is already the default - Bionic's math functions do not set errno and clang
# knows it, so sqrt() compiles to a bare fsqrt either way. Verified, not assumed: adding the
# flag produces byte-identical .so files for all four ABIs.
#
# -fvisibility=hidden is load-bearing here in a way it is not on Windows. These are ELF shared
# objects, so without it every non-static symbol would land in .dynsym; with it, only the
# JNIEXPORT ones do. Result: exactly the 106 JNI entry points are exported (verified per ABI by
# tools/verify-contract.sh).
LOCAL_CFLAGS    += -O2
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections -fvisibility=hidden -Wall -Wno-error=implicit-function-declaration

# -z max-page-size=16384: required for Android 15+, which can run with 16 KB memory pages.
# A library linked for 4 KB pages fails to load there.
# --gc-sections pairs with the -ffunction-sections/-fdata-sections above.
#
# (Those two used to be repeated in LOCAL_LDFLAGS as well. They are codegen flags: the clang
# driver accepts and ignores them when it is only linking, so that had no effect either way.)
LOCAL_LDFLAGS   += -Wl,-z,max-page-size=16384,--gc-sections

LOCAL_SRC_FILES := swedate.c swehouse.c swejpl.c swemmoon.c swemplan.c sweph.c swephlib.c swecl.c swehel.c swejni.c
include $(BUILD_SHARED_LIBRARY)
