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

# minSdk. Bumping this is an app-compatibility decision, not a build tuning one.
APP_PLATFORM := android-21

# release: -DNDEBUG and the per-ABI optimisation preset. Android.mk overrides the -O level;
# see the comment there for why armeabi-v7a needed it.
APP_OPTIM := release
