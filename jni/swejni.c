#include "swephexp.h"
#include "swejni.h"

void appendToBuilder(JNIEnv *env, char *chArray, jobject builder) {
	// Obtain the Java StringBuilder class handle
	jclass clazz = (*env)->GetObjectClass(env, builder);

	// Obtain the method ID for the StringBuilder append method
	jmethodID mid = (*env)->GetMethodID(env, clazz, "append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;");

	// If this method does not exist then return.
	if (mid == 0) return;

	// Create a new Java String object for the given char array
	jstring jArray = (*env)->NewStringUTF(env, chArray);

	// Call the StringBuilder object's append method
	(*env)->CallObjectMethod(env, builder, mid, jArray);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1ephe_1path(JNIEnv *env, jclass swephexp, jstring jEphePath) {
	jboolean isEphePathCopy;
	char ephe_path[AS_MAXCH];
	
	const char *ephePath = (*env)->GetStringUTFChars(env, jEphePath, &isEphePathCopy);
	strcpy(ephe_path, ephePath);
	
	if (isEphePathCopy == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, jEphePath, ephePath);
	
	swe_set_ephe_path(ephe_path);
}

/* set file name of JPL file */
JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1jpl_1file(JNIEnv *env, jclass swephexp, jstring jplPath) {
	jboolean isJplPathCopy;
	char fname[AS_MAXCH];

	const char *jplFile = (*env)->GetStringUTFChars(env, jplPath, &isJplPathCopy);
	strcpy(fname, jplFile);

	if (isJplPathCopy == JNI_TRUE) (*env)->ReleaseStringUTFChars(env, jplPath, jplFile);

	swe_set_jpl_file(fname);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1topo(JNIEnv *env, jclass swephexp, jdouble geolon, jdouble geolat, jdouble geoalt) {
	swe_set_topo(geolon, geolat, geoalt);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1sid_1mode(JNIEnv *env, jclass swephexp, jint sid_mode, jdouble t0, jdouble ayan_t0) {
	swe_set_sid_mode(sid_mode, t0, ayan_t0);
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1sidtime(JNIEnv *env, jclass swephexp, jdouble tjd_ut) {
	return swe_sidtime(tjd_ut);
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1sidtime0
  (JNIEnv *env, jclass swephexp, jdouble tjd_ut, jdouble eps, jdouble nut) {
	return swe_sidtime0(tjd_ut, eps, nut);
}

JNIEXPORT jstring JNICALL Java_swisseph_SwephExp_swe_1get_1planet_1name(JNIEnv *env, jclass swephexp, jint ipl) {
	char pnameArr[256];
	jstring result = NULL;
	const char * pname = swe_get_planet_name(ipl, pnameArr);
	if ( NULL != pname) result = (*env)->NewStringUTF(env, pname);
	return result;
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1get_1ayanamsa_1ut(JNIEnv *env, jclass swephexp, jdouble tjd_ut) {
	return swe_get_ayanamsa_ut(tjd_ut);
}

JNIEXPORT jstring JNICALL Java_swisseph_SwephExp_swe_1get_1ayanamsa_1name(JNIEnv *env, jclass swephexp, jint isidmode) {
	jstring result = NULL;
	const char *ayanamsa_name = swe_get_ayanamsa_name(isidmode);
	if ( NULL != ayanamsa_name) result = (*env)->NewStringUTF(env, ayanamsa_name);
	return result;
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1get_1ayanamsa(JNIEnv *env, jclass swephexp, jdouble tjd_et) {
	return swe_get_ayanamsa(tjd_et);
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1houses(JNIEnv *env, jclass swephexp, jdouble tjd_ut,
jdouble geolat, jdouble geolon, jint hsys, jdoubleArray cuspsArray, jdoubleArray ascmcArray) {
	jboolean isCuspsCopy, isAscmcCopy;

	jdouble *cusps = (*env)->GetDoubleArrayElements(env, cuspsArray, &isCuspsCopy);
	jdouble *ascmc = (*env)->GetDoubleArrayElements(env, ascmcArray, &isAscmcCopy);

	int retc = swe_houses(tjd_ut, geolat, geolon, hsys, cusps, ascmc);

	/*
	 * 0 Copy the contents of the buffer back into array and free the buffer
	 * JNI_ABORT	Free the buffer without copying back any changes
	 * JNI_COMMIT	Copy the contents of the buffer back into array but do not free buffer
	 * */
	if (isCuspsCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, cuspsArray, cusps, 0);
	if (isAscmcCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, ascmcArray, ascmc, 0);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1houses_1ex(JNIEnv *env, jclass swephexp, jdouble tjd_ut,
jint iflag, jdouble geolat, jdouble geolon, jint hsys, jdoubleArray cuspsArray, jdoubleArray ascmcArray) {
	jboolean isCuspsCopy, isAscmcCopy;

	jdouble *cusps = (*env)->GetDoubleArrayElements(env, cuspsArray, &isCuspsCopy);
	jdouble *ascmc = (*env)->GetDoubleArrayElements(env, ascmcArray, &isAscmcCopy);

	int retc = swe_houses_ex(tjd_ut, iflag, geolat, geolon, hsys, cusps, ascmc);
	if (isCuspsCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, cuspsArray, cusps, 0);
	if (isAscmcCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, ascmcArray, ascmc, 0);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1calc(JNIEnv *env, jclass swephexp, jdouble tjd, jint ipl,
jint iflag, jdoubleArray xxArray, jobject errBuilder) {
	jboolean isXxCopy;
	char serr[256];

	jdouble *xx = (*env)->GetDoubleArrayElements(env, xxArray, &isXxCopy);
	int retc = swe_calc(tjd, ipl, iflag, xx, serr);

	if (isXxCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, xxArray, xx, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1calc_1ut(JNIEnv *env, jclass swephexp, jdouble tjd_ut, jint ipl,
jint iflag, jdoubleArray xxArray, jobject errBuilder) {
	jboolean isXxCopy;
	char serr[256];

	jdouble *xx = (*env)->GetDoubleArrayElements(env, xxArray, &isXxCopy);
	int retc = swe_calc_ut(tjd_ut, ipl, iflag, xx, serr);

	if (isXxCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, xxArray, xx, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1sol_1eclipse_1when_1loc(JNIEnv *env, jclass swephexp, jdouble tjd_start,
jint ifl, jdoubleArray geoposArray, jdoubleArray tretArray, jdoubleArray attrArray, jint backward, jobject errBuilder) {
	char serr[256];
	jboolean isGeoposCopy, isTretCopy, isAttrCopy;

	jdouble *geopos = (*env)->GetDoubleArrayElements(env, geoposArray, &isGeoposCopy);
	jdouble *tret = (*env)->GetDoubleArrayElements(env, tretArray, &isTretCopy);
	jdouble *attr = (*env)->GetDoubleArrayElements(env, attrArray, &isAttrCopy);

	int retc = swe_sol_eclipse_when_loc(tjd_start, ifl, geopos, tret, attr, backward, serr);

	if (isGeoposCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, geoposArray, geopos, 0);
	if (isTretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tretArray, tret, 0);
	if (isAttrCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, attrArray, attr, 0);

	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1lun_1eclipse_1when_1loc(JNIEnv *env, jclass swephexp, jdouble tjd_start,
jint ifl, jdoubleArray geoposArray, jdoubleArray tretArray, jdoubleArray attrArray, jint backward, jobject errBuilder) {
	char serr[256];
	jboolean isGeoposCopy, isTretCopy, isAttrCopy;

	jdouble *geopos = (*env)->GetDoubleArrayElements(env, geoposArray, &isGeoposCopy);
	jdouble *tret = (*env)->GetDoubleArrayElements(env, tretArray, &isTretCopy);
	jdouble *attr = (*env)->GetDoubleArrayElements(env, attrArray, &isAttrCopy);

	int retc = swe_lun_eclipse_when_loc(tjd_start, ifl, geopos, tret, attr, backward, serr);

	if (isGeoposCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, geoposArray, geopos, 0);
	if (isTretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tretArray, tret, 0);
	if (isAttrCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, attrArray, attr, 0);

	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1sol_1eclipse_1when_1glob(JNIEnv *env, jclass swephexp,
jdouble tjd_start, jint ifl, jint ifltype, jdoubleArray tretArray, jint backward, jobject errBuilder) {
	char serr[256];
	jboolean isTretCopy;

	jdouble *tret = (*env)->GetDoubleArrayElements(env, tretArray, &isTretCopy);
	int retc = swe_sol_eclipse_when_glob(tjd_start, ifl, ifltype, tret, backward, serr);
	if (isTretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tretArray, tret, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1lun_1eclipse_1when(JNIEnv *env, jclass swephexp, jdouble tjd_start,
jint ifl, jint ifltype, jdoubleArray tretArray, jint backward, jobject errBuilder) {
	char serr[256];
	jboolean isTretCopy;

	jdouble *tret = (*env)->GetDoubleArrayElements(env, tretArray, &isTretCopy);
	int retc = swe_lun_eclipse_when(tjd_start, ifl, ifltype, tret, backward, serr);
	if (isTretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tretArray, tret, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1rise_1trans(JNIEnv *env, jclass swephexp, jdouble tjd_ut, jint ipl,
jobject starnameBuilder, jint epheflag, jint rsmi, jdoubleArray geoposArray, jdouble atpress, jdouble attemp, jdoubleArray tretArray, jobject errBuilder) {
	char serr[256];
	char* starName = NULL;
	jboolean isGeoposCopy, isTretCopy;

	/*
	if ( NULL != jStarName ) {
		char starNameArray[256];
		const char* constStarName = (*env)->GetStringUTFChars(jStarName, &isStarNameCopy);
		sprintf(starNameArray, "%s", constStarName);
		(*env)->ReleaseStringUTFChars(jStarName, constStarName);
		starName = starNameArray;
	}*/

	jdouble *geopos = (*env)->GetDoubleArrayElements(env, geoposArray, &isGeoposCopy);
	jdouble *tret = (*env)->GetDoubleArrayElements(env, tretArray, &isTretCopy);

	int retc = swe_rise_trans(tjd_ut, ipl, starName, epheflag, rsmi, geopos, atpress, attemp, tret, serr);

	if (isGeoposCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, geoposArray, geopos, 0);
	if (isTretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tretArray, tret, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1date_1conversion
  (JNIEnv *env, jclass swephexp, jint y, jint m, jint d, jdouble utime, jchar calendarType, jdoubleArray tjdArray) {
	jboolean isTjdCopy;
	jdouble *tjd = (*env)->GetDoubleArrayElements(env, tjdArray, &isTjdCopy);

	/* calendarType - g[regorian]|j[ulian] */
	/* utime - universal time in hours (decimal) */

	int retc = swe_date_conversion(y, m, d, utime, calendarType, tjd);
	if (isTjdCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, tjdArray, tjd, 0);

	return retc;
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1julday
  (JNIEnv *env, jclass swephexp, jint year, jint month, jint day, jdouble hour, jint gregflag) {
	return swe_julday(year, month, day, hour, gregflag);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1revjul
  (JNIEnv *env, jclass swephexp, jdouble jd, jint gregflag, jintArray ymdArray, jdoubleArray utimeArray) {
	jboolean isYmdCopy, isUtimeCopy;

	int ymd[3];
	jint *ymdOut = (*env)->GetIntArrayElements(env, ymdArray, &isYmdCopy);
	jdouble *utime = (*env)->GetDoubleArrayElements(env, utimeArray, &isUtimeCopy);

	swe_revjul(jd, gregflag, &ymd[0], &ymd[1], &ymd[2], utime);
	ymdOut[0] = ymd[0]; ymdOut[1] = ymd[1]; ymdOut[2] = ymd[2];

	if (isYmdCopy == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, ymdArray, ymdOut, 0);
	if (isUtimeCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, utimeArray, utime, 0);
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1deltat
(JNIEnv *env, jclass swephexp, jdouble tjd) {
	return swe_deltat(tjd);
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1deltat_1ex
(JNIEnv *env, jclass swephexp, jdouble tjd, jint iflag, jobject errBuilder) {
	return swe_deltat_ex(tjd, iflag, NULL);
}

JNIEXPORT jdouble JNICALL Java_swisseph_SwephExp_swe_1get_1tid_1acc
  (JNIEnv *env, jclass swephexp) {
	return swe_get_tid_acc();
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1tid_1acc
  (JNIEnv *env, jclass swephexp, jdouble t_acc) {
	swe_set_tid_acc(t_acc);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1set_1delta_1t_1userdef
  (JNIEnv *env, jclass swephexp, jdouble dt) {
	swe_set_delta_t_userdef(dt);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1close(JNIEnv *env, jclass swephexp) {
	swe_close();
}

JNIEXPORT jstring JNICALL Java_swisseph_SwephExp_swe_1house_1name(JNIEnv *env, jclass swephexp, jint hsys) {
	jstring result = NULL;
	char *house_name = swe_house_name(hsys);
	if ( NULL != house_name ) result = (*env)->NewStringUTF(env, house_name);
	return result;
}

JNIEXPORT jint JNICALL Java_swisseph_SwephExp_swe_1utc_1to_1jd(JNIEnv *env, jclass swephexp,jint iyear,jint imonth,
jint iday, jint ihour, jint imin, jdouble dsec, jint gregflag, jdoubleArray dretArray, jobject errBuilder) {
	char serr[256];
	jboolean isDretCopy;

	jdouble *dret = (*env)->GetDoubleArrayElements(env, dretArray, &isDretCopy);
	int retc = swe_utc_to_jd(iyear, imonth, iday, ihour, imin, dsec, gregflag, dret, serr);

	if (isDretCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, dretArray, dret, 0);
	if ( 0 > retc ) appendToBuilder(env, serr, errBuilder);

	return retc;
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1jdet_1to_1utc
  (JNIEnv *env, jclass swephexp, jdouble tjd_et, jint gregflag, jintArray ymdhmArray, jdoubleArray dsecArray) {
	jboolean isYmdhmCopy, isDsecCopy;
	jint *ymdhmOut = (*env)->GetIntArrayElements(env, ymdhmArray, &isYmdhmCopy);
	jdouble *dsec = (*env)->GetDoubleArrayElements(env, dsecArray, &isDsecCopy);

	int ymdhm[5];
	swe_jdet_to_utc(tjd_et, gregflag, &ymdhm[0], &ymdhm[1], &ymdhm[2], &ymdhm[3], &ymdhm[4], dsec);

	if (isDsecCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, dsecArray, dsec, 0);
	if (isYmdhmCopy == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, ymdhmArray, ymdhmOut, 0);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1jdut1_1to_1utc
  (JNIEnv *env, jclass swephexp, jdouble tjd_ut, jint gregflag, jintArray ymdhmArray, jdoubleArray dsecArray) {
	jboolean isYmdhmCopy, isDsecCopy;
	jint *ymdhmOut = (*env)->GetIntArrayElements(env, ymdhmArray, &isYmdhmCopy);
	jdouble *dsecOut = (*env)->GetDoubleArrayElements(env, dsecArray, &isDsecCopy);

	int ymdhm[5];
	swe_jdut1_to_utc(tjd_ut, gregflag, &ymdhm[0], &ymdhm[1], &ymdhm[2], &ymdhm[3], &ymdhm[4], dsecOut);
	ymdhmOut[0] = ymdhm[0]; ymdhmOut[1] = ymdhm[1]; ymdhmOut[2] = ymdhm[2];
	ymdhmOut[3] = ymdhm[3]; ymdhmOut[4] = ymdhm[4];

	if (isDsecCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, dsecArray, dsecOut, 0);
	if (isYmdhmCopy == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, ymdhmArray, ymdhmOut, 0);
}

JNIEXPORT void JNICALL Java_swisseph_SwephExp_swe_1utc_1time_1zone(JNIEnv *env, jclass swephexp, jint iyear, jint imonth,
jint iday, jint ihour, jint imin, jdouble dsec, jdouble d_timezone, jintArray ymdhmArray, jdoubleArray dsecArray) {
	jboolean isYmdhmCopy, isDsecCopy;
	jint *ymdhmOut = (*env)->GetIntArrayElements(env, ymdhmArray, &isYmdhmCopy);
	jdouble *dsecOut = (*env)->GetDoubleArrayElements(env, dsecArray, &isDsecCopy);

	int ymdhm[5];
	swe_utc_time_zone(iyear, imonth, iday, ihour, imin, dsec, d_timezone,
		&ymdhm[0], &ymdhm[1], &ymdhm[2], &ymdhm[3], &ymdhm[4], dsecOut);

	ymdhmOut[0] = ymdhm[0]; ymdhmOut[1] = ymdhm[1]; ymdhmOut[2] = ymdhm[2];
	ymdhmOut[3] = ymdhm[3]; ymdhmOut[4] = ymdhm[4];

	if (isDsecCopy == JNI_TRUE) (*env)->ReleaseDoubleArrayElements(env, dsecArray, dsecOut, 0);
	if (isYmdhmCopy == JNI_TRUE) (*env)->ReleaseIntArrayElements(env, ymdhmArray, ymdhmOut, 0);
}
