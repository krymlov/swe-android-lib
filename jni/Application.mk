# All four ABIs Google Play still accepts. armeabi-v7a and x86 are 32-bit and increasingly rare,
# but dropping either is a distribution decision, not a build one.
#
# Note that these do not compute bit-identical results to each other, and cannot be made to
# without a flag change: arm64-v8a has FMA in its base ISA and clang contracts a*b+c into a
# single fmadd by default (~1900 of them in this library), so it rounds once where the other
# three round twice. The difference is around 1e-11 degrees - far below the 1e-6 tolerance the
# instrumented reference test uses, and arguably more accurate rather than less - but it does
# mean arm64-v8a is not bit-identical to the desktop DLL the way the other ABIs are.
# -ffp-contract=off would close that gap at a cost in speed. See CLAUDE.md.
APP_ABI := armeabi-v7a arm64-v8a x86_64 x86

# Not lower than 24: swejpl.c builds with _FILE_OFFSET_BITS 64, and on the 32-bit ABIs bionic
# declares fseeko/ftello (as fseeko64/ftello64) only from API 24. Below that they were implicit
# declarations called with a 64-bit offset where libc takes a 32-bit one, so every seek in a
# JPL file went wrong on armeabi-v7a and x86. The .so does not load below API 24 now, so
# minSdk (AAR_MIN_SDK) has to be at least 24 as well.
APP_PLATFORM := android-24

# release: -DNDEBUG and the per-ABI optimisation preset. Android.mk overrides the -O level;
# see the comment there for why armeabi-v7a needed it.
APP_OPTIM := release
