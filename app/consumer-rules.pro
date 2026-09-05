# Proguard rules for consumers of the swe-android-lib library

# The JNI entry points are resolved by symbol name, so neither the class nor the native
# methods may be renamed. (The default proguard-android-optimize.txt already carries the
# generic rule; this states it explicitly for the classes this library owns.)
-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class swisseph.SwephExp {
    native <methods>;
}
