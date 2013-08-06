CFLAGS   = -I $(JAVA_HOME)/include -I $(JAVA_HOME)/include/linux
JNI_NAME = pty
PROGNAME=libpty.so

SRCS = pty.c


CFLAGS += -Wall

all:
	gcc $(CFLAGS) -o $(PROGNAME) -fPIC -shared $(SRCS) -lc
#	gcc $(CFLAGS) -o $(PROGNAME) -fPIC -shared $(SRCS) -Wl,soname,lib$(JNI_NAME) -lc

clean:
	rm -f lib$(JNI_NAME).so
