/*
 * exceptions.h
 *
 *  Created on: 29.02.2012
 *      Author: andre
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <string.h>
#include <stdlib.h>
#include <errno.h>


static jint throwIOException(JNIEnv *env, int errnum, const char *msg);
static jint throwNoClassDefError(JNIEnv *env, const char *msg);
static jint throwNoSuchFieldError(JNIEnv *env, const char *msg);
static jint throwOutOfMemoryError(JNIEnv *env, const char *msg);


static jint throwIOException(JNIEnv *env, int errnum, const char *msg) {
	jclass exClass;
	int intret;
	char *className = "java/io/IOException";
	char *errMsgDetailed = NULL;
	char *errMsg = NULL;

	if (errnum != 0) {
		errMsgDetailed = strerror(errnum);
	}

	errMsg = (char *) malloc(strlen(msg) + strlen(errMsgDetailed) + strlen(": "));
	if (errMsg == NULL)
		return throwOutOfMemoryError(env, NULL);

	sprintf(errMsg, "%s: %s", msg, errMsgDetailed);

	exClass = (*env)->FindClass(env, className);
	if (exClass == NULL)
        return throwNoClassDefError(env, className);

	intret = (*env)->ThrowNew(env, exClass, errMsg);
	free(errMsg);

	return intret;
}

static jint throwNoClassDefError(JNIEnv *env, const char *msg) {
	jclass exClass;
	char *className = "java/lang/NoClassDefFoundError";

	exClass = (*env)->FindClass(env, className);
	if (exClass == NULL)
        return throwNoClassDefError(env, className);

	return (*env)->ThrowNew(env, exClass, msg);
}


static jint throwNoSuchFieldError(JNIEnv *env, const char *msg) {
	jclass exClass;
	char *className = "java/lang/NoSuchFieldError";

	exClass = (*env)->FindClass(env, className);
	if (exClass == NULL)
        return throwNoClassDefError(env, className);

	return (*env)->ThrowNew(env, exClass, msg);
}

static jint throwOutOfMemoryError(JNIEnv *env, const char *msg) {
	static jclass exClass;
	static char *className = "java/lang/OutOfMemoryError";

	exClass = (*env)->FindClass(env, className);
	if ( exClass == NULL )
        return throwNoClassDefError(env, className );

	return (*env)->ThrowNew(env, exClass, msg);
}


#endif /* EXCEPTIONS_H_ */
