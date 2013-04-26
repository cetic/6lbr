/*
 *
 * 2012-03-01
 * Andre Frimberger:
 *   * Implemented pseudo terminal support for Java
 *
 *
 * Copyright (c) 2012, Andre Frimberger
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#define _XOPEN_SOURCE 600
#define __USE_BSD

#include "include/LinuxPseudoTerminal.h"
#include "include/exceptions.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <termios.h>
#include <termio.h>
#include <pty.h>
#include <fcntl.h>



int g_fdslave = -1;


int FileDescriptor_get_fd(JNIEnv *env, jobject obj_java_io_FileDescriptor) {
	jclass classFileDescriptor = NULL;
	jobject objfdmaster = obj_java_io_FileDescriptor;
	jfieldID fieldfd = NULL;
	jint intret;

	if( !(classFileDescriptor = (*env)->GetObjectClass(env, obj_java_io_FileDescriptor))) {
		throwNoClassDefError(env, "Class java.io.FileDescriptor not found");
		return -1;
	}

	if( !(fieldfd = (*env)->GetFieldID(env, classFileDescriptor, "fd", "I" ))) {
		throwNoSuchFieldError(env, "Field 'I fd' not found");
		return -1;
	}

	intret = (*env)->GetIntField(env, objfdmaster, fieldfd);
	//fprintf(stderr, "get filedescriptor: %i \n", intret);
	return intret;
}


jobject FileDescriptor_set_fd(JNIEnv *env, jobject obj_java_io_FileDescriptor, int fd) {

	jclass classFileDescriptor = NULL;
	jobject objfdmaster = obj_java_io_FileDescriptor;
	jfieldID fieldfd = NULL;

	if( !(classFileDescriptor = (*env)->GetObjectClass(env, obj_java_io_FileDescriptor))) {
		throwNoClassDefError(env, "Error getting Class for Object obj_java_io_FileDescriptor");
		return NULL;
	}

	if( !(fieldfd = (*env)->GetFieldID(env, classFileDescriptor, "fd", "I" ))) {
		throwNoSuchFieldError(env, "Field 'I fd' not found");
		return NULL;
	}
	(*env)->SetIntField(env, objfdmaster, fieldfd, fd);

	return obj_java_io_FileDescriptor;
}




JNIEXPORT void JNICALL
Java_de_fau_cooja_plugins_LinuxPseudoTerminal_setupMasterPt (JNIEnv *env, jobject obj, jobject obj_java_io_FileDescriptor) {
	int fdmaster, retint;
	struct termios oldtios; // for saving current terminal settings
	struct termios newtios;

	//fprintf(stderr, "======================= setupMasterPt ======================= \n");
	fdmaster = posix_openpt(O_RDWR);
	if (fdmaster < 0) { throwIOException(env, errno, "ERROR: calling posix_openpt()"); return; }

	// change mode and owner of slave pty
	retint = grantpt(fdmaster);
	if (retint != 0) { throwIOException(env, errno, "ERROR: calling grantpt()"); return; }

	// unlock slave pty for others
	retint = unlockpt(fdmaster);
	if (retint != 0) { throwIOException(env, errno, "ERROR: calling unlockpt()"); return; }


	// save current settings
	retint = tcgetattr(fdmaster, &oldtios);
	newtios = oldtios;

	// set raw mode (equivalent to: cfmakeraw(&newtios);
	newtios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	newtios.c_oflag &= ~OPOST;
	newtios.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	newtios.c_cflag &= ~(CSIZE|PARENB);
	newtios.c_cflag |= CS8;
	tcsetattr(fdmaster, TCSANOW, &newtios);

	//fprintf(stderr, "setting file descriptor - current value %i \n", FileDescriptor_get_fd(env, obj_java_io_FileDescriptor));
	FileDescriptor_set_fd(env, obj_java_io_FileDescriptor, fdmaster);
	//fprintf(stderr, "setupMasterPt: masterpty has fd: %i \n", FileDescriptor_get_fd(env, obj_java_io_FileDescriptor));
}


JNIEXPORT jstring JNICALL
Java_de_fau_cooja_plugins_LinuxPseudoTerminal_getSlavePtsName(JNIEnv *env, jobject obj, jobject obj_fdmaster) {
	//fprintf(stderr, "getSlavePtsName: returning name for fd: %i \n", FileDescriptor_get_fd(env, obj_fdmaster));
	return (*env)->NewStringUTF(env, ptsname(FileDescriptor_get_fd(env, obj_fdmaster)));
}


JNIEXPORT void JNICALL
Java_de_fau_cooja_plugins_LinuxPseudoTerminal_setupSlavePt(JNIEnv *env, jobject obj, jobject obj_fdmaster) {
	int fdslave;

	// open the slave pty to be sure it stays open
	fdslave = open(ptsname(FileDescriptor_get_fd(env, obj_fdmaster)), O_RDWR);
	if (fdslave < 0) { throwIOException(env, errno, "ERROR: opening master pty failed"); return;}

	g_fdslave = fdslave;
}


JNIEXPORT void JNICALL
Java_de_fau_cooja_plugins_LinuxPseudoTerminal_closePt(JNIEnv *env, jobject obj, jobject obj_fdmaster) {
	int fdmaster;

	if (g_fdslave != -1) close(g_fdslave);

	fdmaster = FileDescriptor_get_fd(env, obj_fdmaster);
	close(fdmaster);
}
