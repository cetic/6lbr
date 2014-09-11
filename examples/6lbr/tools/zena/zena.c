/**
 * Zena Linux - a command line utility to interact with the Microchip
 * Technologies ZENA 2.5GHz 802.15.4 packet sniffer. Project hosted at
 * http://code.google.com/p/microchip-zena/
 *
 * Copyright (c) 2011,2012, Joe Desbonnet, jdesbonnet@gmail.com
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * The name of the contributors may not be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Special thanks to Joshua Wright who did much of the initial reverse 
 * engineering work on the Microchip ZENA. See this post for details:
 * http://www.willhackforsushi.com/?p=198
 *
 * Version 0.1 (16 Feb 2011)
 * First release. Used libusb 0.1.
 *
 * Version 0.2 (19 Feb 2011) 
 * Identical in functionality to verion 0.1 except that it
 * uses libusb version 1.0 API (the previous version used libusb v0.1). 
 *
 * Version 0.3 (25 Feb 2011)  (CVS file version 1.52)
 * * Add 802.15.4 channel to usbhex records as the second item in the
 * record after the packet timestamp (in hex). This ensure that all
 * packet data and metadata (reception, channel, timestamp) is
 * recorded. 
 * * If ZENA is bound to a kernel driver, it will attempt to detach
 * it from the kernel driver. Up do now this had to be done manually
 * prior to running this utility.
 * * Use host timestamp in pcap file instead of ZENA timestamp.
 * * -s <t> switch to scan through 802.15.4 channels, where t = channel
 * time slice in ms.
 *
 * Version 0.4 (1 Mar 2011)  (CVS file version 1.61)
 * * Add signal handler for graceful exit.
 * * Buffer entire 802.15.4 packet and check if suitable for outputting
 * to pcap file. Drop corrupted packets by default. Use -b to override.
 * * Use -q to suppress warning messages.
 *
 * Version 0.4.1 (20 Mar 2011) (CVS file version 1.63)
 * * Remove call to zena_get_packet() just before the output format switch
 * statement in the main loop. This was unnecessary and would have resulted
 * in lost packets.
 *
 * Version 0.4.2 (2 Feb 2012) (CVS file version 1.68)
 * Change way packets with bad FCS are handled when writing PCAP. There was 
 * bug where the packet written to the PCAP file was two bytes shorter than 
 * that declared in the header when FCS was bad and 'drop bad packets' flag
 * was disabled.
 *
 * Version 0.4.3 (16 Feb 2012) (CVS file version 1.70)
 * Check zena_packet.packet_len is a sane value. Occasionally getting crazy
 * lengths which causes SEGV when accessing the zena_packet.packet[] buffer.
 *
 * TODO: 
 * * Option to use ZENA or host timestamp 
 *
 * Requires libusb-1.0 (to run) and libusb-1.0-dev (to compile) packages. 
 *
 * To compile:
 * gcc -o zena zena.c -lusb-1.0 -lrt
 *
 * Known issue: can cause Ubuntu 10.x running Linux 2.6.32-* to kernel crash! 
 * Cause unknown. A fresh Ubuntu 10.10 installed from CD running 2.6.35-22 
 * does not seem to have this problem. Suggest running in a virtual machine.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <libusb-1.0/libusb.h>

#define APP_NAME "zena"
#define VERSION "0.4.3, 16 Feb 2012"
#define USB_VENDOR_ID 0x04d8   // Microchip Technologies Inc
#define USB_PRODUCT_ID 0x000e  // ZENA
#define INTERFACE 0

#define FORMAT_PCAP 1
#define FORMAT_USBHEX 2

#define TRUE 1
#define FALSE 0

// Used by zena_get_packet() to return 802.15.4 packet data
typedef struct  {
	int zena_ts_sec;	// time stamp reported by ZENA (seconds)
	int zena_ts_usec;	// time stamp reported by ZENA (microseconds)
	int host_ts_sec;	// time stamp reported by host (seconds)
	int host_ts_usec;	// time stamp reported by host (microseconds)
	int packet_len;		// 802.15.4 packet len (excluding FCS)
	uint8_t packet[128];
	uint8_t rssi;
	uint8_t lqi;
	uint8_t fcs_ok;		// set to TRUE (1) if FCS ok, else FALSE (0)
} zena_packet_t;
	

const static int TIMEOUT=200; // Default USB timeout in ms
const static int PACKET_FRAG_TIMEOUT = 100; // USB timeout when retrieving 2nd or 3rd part of packet
 
// Define USB endpoints
const static int ZENA_EP_PACKETS = 0x81 ;	// where we get packet data
const static int ZENA_EP_CONTROL = 0x01 ;	// where we configure the device

// PCAP constants
const static int PCAP_MAGIC = 0xa1b2c3d4;
const static short PCAP_VERSION_MAJOR = 2;
const static short PCAP_VERSION_MINOR = 4;
const static int PCAP_TZ = 0;				// thiszone: GMT to local correction
const static int PCAP_SIGFIGS = 0;			// sigfigs: accuracy of timestamps
const static int PCAP_SNAPLEN = 128;		// snaplen: max len of packets, in octets
const static int PCAP_LINKTYPE = 0xc3;		// data link type DLT_IEEE802_15_4 (see <pcap/bpf.h>)


// Define error codes used internally
const static int ERR_INVALID_CHANNEL = -60 ;

// The debug level set with the -d command line switch
int debug_level = 0;

// Timeout used when reading packet data in milliseconds. Changed during
// packet scan to be the timeslice interval.
int usb_timeout = 200;

// Flag set to true if a kernel driver detach was performed. 
// Allows for reattach in exit handler.
int kernel_driver_detach = FALSE;

// Use -q flag to enable quiet mode. Warning messages will be suppressed.
int quiet_mode = FALSE;

// Set to true in signal_handler to signal exit from main loop
int exit_flag = FALSE;

libusb_device *find_zena();
void debug (int level, const char *msg, ...);
void warning (const char *msg, ...);

/**
 * Locate and setup ZENA device on USB bus. Return libusb_device if successfully
 * found and setup. Return NULL if some error condition.
 *
 * @return libusb_device USB device handle for ZENA device or NULL if error condition.
 */
libusb_device_handle *setup_libusb_access() {

	int status;
	libusb_device_handle *zena = NULL;

	// libusb API 1.0 documentation here:
	// http://libusb.sourceforge.net/doc/function.usbsetconfiguration.html

	// Initialize libusb library. libusb_init() must be called before any 
	// other libusb_* function. If parameter is NULL use default libusb_context.
	// http://libusb.sourceforge.net/api-1.0/group__lib.html
	debug (1, "calling libusb_init() to initialize libusb");	
	status = libusb_init (NULL);
	if ( status < 0 ) {
		fprintf (stderr,"ERROR: Could not initialize libusb\n");
		return NULL;
	}

	// Set debugging level 0 .. 3. NULL param means use default usb context
	libusb_set_debug (NULL, (debug_level == 0 ? 0 : 3) );

	debug (1, "calling libusb_open_device_with_vid_pid() to open USB device handle to ZENA");
	zena = libusb_open_device_with_vid_pid (NULL, USB_VENDOR_ID, USB_PRODUCT_ID);
	if (zena == NULL) {
		fprintf (stderr,"ERROR: Could not open ZENA device. Not found or not accessible.\n");
		return NULL;
	}

	// Check if a kernel driver is attached to the device. Detach it if so.
	if (libusb_kernel_driver_active(zena,INTERFACE)) {
		warning("Kernel driver bound to ZENA. Attempting to detach.\n");
		debug(9,"calling libusb_detach_kernel_driver() to detach kernel driver from ZENA");
		status = libusb_detach_kernel_driver(zena,INTERFACE);
		if ( status < 0 ) {
			fprintf (stderr,"ERROR: could not detach kernel driver from ZENA, errorCode=%d",status);
			return NULL;
		}
		kernel_driver_detach = TRUE;
		debug (9,"kernel driver detach successful.\n");
	}
	
	// From "lsusb -v" bConfigurationValue is 1.
	// TODO: what does this mean?
	debug (9, "calling usb_set_configuration()");
	status = libusb_set_configuration(zena, 1);
	if ( status < 0 ) {
		fprintf(stderr,"ERROR: Could not set configuration 1: errorCode=%d\n", status);
		return NULL;
	}
 
	// Claim interface. This is problematic. When ZENA is first plugged in
	// something in the OS automatically 'binds' it causing this to fail.
	// Can we programatically 'unbind' it? Maybe with usb_release_interface()?
	debug (1, "calling libusb_claim_interface(%d)",INTERFACE);
	status = libusb_claim_interface(zena, INTERFACE);
	if ( status < 0) {
		fprintf(stderr,"ERROR: Could not claim interface %d: errorCode=%d. Is device already bound?\n",INTERFACE,status);
		return NULL;
	}
 
	// Success, return usb_dev_handle
	debug (1,"ZENA successfully located and claimed");
	return zena;
}
 

/**
 * Select 802.15.4 channel on ZENA. 
 * 
 * Empty packet buffers before changing channel (otherwise
 * it won't be clear from which channel a packet arrived).
 *
 * @param zena The libusb_device_handle of ZENA device
 * @param channel The 802.15.4 channel. Must be 11 to 26.
 *
 * @return int Return 0 if successful or error code < 0 if error condition.
 * ERR_INVALID_CHANNEL: channel out of allowed range. Must be >= 11 and <= 26.
 */
int zena_set_channel (libusb_device_handle *zena, int channel) {

	debug (1,"zena_set_channel(), 802.15.4 channel = %d", channel);

	// Check if valid channel
	if (channel < 11 || channel > 26) {
		return ERR_INVALID_CHANNEL;
	}

	// Require a 64 byte buffer to send USB packet to ZENA
	unsigned char usbbuf[64];

	// Number of bytes actually transferred stored here. Not used.
	int status,nbytes;


	//libusb_reset_device(zena);


	// set buffer to all zero
	bzero (usbbuf,64);		

	// Channel is byte offset 1 in packet
	usbbuf[1] = channel;

	// Send to device ZENA_EP_CONTROL end point.
	// http://libusb.sourceforge.net/api-1.0/group__syncio.html
	debug (1, "calling libusb_interrupt_transfer() to ZENA_EP_CONTROL");
	status = libusb_interrupt_transfer (zena, ZENA_EP_CONTROL, usbbuf, 64, &nbytes, TIMEOUT);
	if ( status < 0 ) {
		fprintf (stderr,"ERROR: zena_set_channel(): error on libusb_interrupt_transfer(). errorCode=%d\n", status);
		return status;
	}

	debug (1, "ZENA is now set to 802.15.4 channel %d", channel);

	// Flush packet buffers by reading. We need to do this because packet
	// may have arrived before channel was changed. We we allow such packets
	// to be outputted it will be tagged with the incorrect 802.15.4 channel.
	// Better to loose this data than have inaccurate data.
	do {
		status = libusb_interrupt_transfer (zena, ZENA_EP_PACKETS, usbbuf, 64, &nbytes, PACKET_FRAG_TIMEOUT);
		if (nbytes>0) {
			debug (9,"found %d bytes in buffer after channel change\n", nbytes);
		}
	} while (nbytes>0);


}



/**
 * Retrieve one 802.15.4 packet from ZENA. This may require multiple 64 byte
 * USB read requests.
 *
 * @param zena libusb_device_handle for ZENA USB device which must be open and ready
 * @param zena_packet A memory structure which will be populated with 802.15.4 packet
 * data and metadata read from the ZENA
 * 
 * @return 0 on success. Negative error code on failure. If an error code is returned
 * the contents of zena_packet is undefined.
 * TODO: mixing up libusb return codes with my own return codes.
 */
int zena_get_packet (libusb_device_handle *zena,  zena_packet_t *zena_packet) {

	int status,nbytes,data_len,packet_len;
	struct timespec tp;
	uint8_t usbbuf[64];

	// http://libusb.sourceforge.net/doc/function.usbinterruptread.html
	// Documentation says status should contain the number of bytes read.
	// This is not what I'm finding. Getting 0 on success.
	//debug (1, "calling usb_interrupt_read()");
	status = libusb_interrupt_transfer(zena, ZENA_EP_PACKETS, usbbuf, 64, &nbytes, usb_timeout);
	// check for timeout and silently ignore
	if (status == LIBUSB_ERROR_TIMEOUT) {
		debug(9,"zena_get_packet(): libusb_interrupt_transfer() timeout");
		return status;
	}

	// a real error (ie not timeout)
	// LIBUSB_ERROR_IO = -1
	if (status < 0) {
		fprintf (stderr,"ERROR: error retrieving ZENA packet, errorCode=%d\n", status);
		return -2;
	}

	// get host time of packet reception
	clock_gettime(CLOCK_REALTIME, &tp);
	

	// Get packet timestamp from ZENA header + capture start time
	zena_packet->host_ts_sec = tp.tv_sec;
	zena_packet->host_ts_usec = tp.tv_nsec / 1000;

	zena_packet->zena_ts_sec = (int)usbbuf[3]  | ( ((int)usbbuf[4])<<8 );
	zena_packet->zena_ts_usec = ( ((int)usbbuf[1])  | ( ((int)usbbuf[2])<<8 )) * 15; //approx
	
	data_len = usbbuf[5];

	// Check for invalid packet lengths
	if (data_len > 129) {
		warning("Packet too long, length=%d. Ignoring.\n",data_len);
		return -3;
	}
	
	// 802.15.4 packet len = data_len -2 because last two bytes from ZENA data are RSSI, LQI/FCS_OK
	//packet_len = data_len - 2; 			

	// Write packet data. This is a little messy because of long packets that don't fit in one
	// chunk of 64 byte USB data.
	if (data_len <= 58) {
		// short packet -- easy!
		memcpy (zena_packet->packet, usbbuf+6, data_len);

	} else {

		memcpy (zena_packet->packet, usbbuf+6, 58);

		debug (1, "calling libusb_interrupt_transfer() for second part of packet");
		status = libusb_interrupt_transfer(zena, ZENA_EP_PACKETS, usbbuf, 64, &nbytes, PACKET_FRAG_TIMEOUT);

		// A status < 0 here will be problematic. Likely that the data will be corrupted. But
		// as the packet header is already written, might as well write what's in the buffer 
		// and display a warning message.
		if (status < 0) {
			warning ("libusb_interrupt_transfer() returned status=%d during second chunk of long packet\n", status);
			return status;
		}

		int bytesRemaining = data_len - 58;
		if ( bytesRemaining <= 63 ) {
			memcpy (zena_packet->packet+58, usbbuf+1, bytesRemaining);
		} else {
			// long packet -- will need third libusb_interrupt_transfer()
			memcpy (zena_packet->packet+58, usbbuf+1, 63);
			bytesRemaining -= 63;
						
			debug (1, "calling libusb_interrupt_transfer() for third part of packet");
 			status = libusb_interrupt_transfer(zena, ZENA_EP_PACKETS, usbbuf, 64, &nbytes, PACKET_FRAG_TIMEOUT);
			// A status < 0 here will be problematic. Likely that the data will be corrupted. But
			// as the packet header is already written, might as well write what's in the buffer 
			// and display a warning message.
			if (status < 0) {
				warning("libusb_interrupt_transfer() returned status=%d during third chunk of long packet\n", status);
				return status;
			}

			memcpy (zena_packet->packet+58+63, usbbuf+1, bytesRemaining);
		}
	}

	zena_packet->rssi = zena_packet->packet[data_len-2];
	zena_packet->lqi = zena_packet->packet[data_len-1]&0x7f;
	zena_packet->fcs_ok = zena_packet->packet[data_len-1]&80 ? TRUE : FALSE;
	zena_packet->packet_len = data_len - 2;

	return 0;
}

/**
 * Display help and usage information. 
 */
void usage () {
	fprintf (stderr,"\n");
	fprintf (stderr,"Usage: zena -c channel [-f format] [-b] [-q] [-v] [-h] [-d level]\n");
	fprintf (stderr,"  -c channel \t Select 802.15.4 channel. Allowed: 11 .. 26\n");
	fprintf (stderr,"  -f format \t Select packet capture format. Allowed: pcap (default) or usbhex.\n");
	fprintf (stderr,"  -d level \t Set debug level, 0 = min [default], 9 = max verbosity\n");
	fprintf (stderr,"  -s interval \t Scan through 802.15.4 channels with timeslice interval in milliseconds\n");
	fprintf (stderr,"  -b \t Include corrupted packets. Applies to pcap output only.\n");
	fprintf (stderr,"  -q \t Quiet mode: suppress warning messages.\n");
	fprintf (stderr,"  -v \t Print version to stderr and exit\n");
	fprintf (stderr,"  -h \t Display this message to stderr and exit\n");

	fprintf (stderr,"\n");
	fprintf (stderr,"Packet capture output is sent to standard output. Use the following command to\n");
	fprintf (stderr,"display real time packet feed in wireshark:\n  wireshark -k -i <( zena -c 20 )\n");

	fprintf (stderr,"\n");
	fprintf (stderr,"Project code and documentation is hosted at:\n  http://code.google.com/p/microchip-zena/\n");
	fprintf (stderr,"\n");
}

void version () {
	fprintf (stderr,"%s, version %s\n", APP_NAME, VERSION);
}

/**
 * Display debug message if suitable log level is selected. 
 * Use vararg mechanism to allow use similar to the fprintf()
 * function.
 *
 * @param level Display this message if log level is greater
 * or equal this level. Otherwise ignore the message.
 * @param msg  Format string as described in fprintf()
 */
void debug (int level, const char* msg, ...) {
	if (level >= debug_level) {
		return;
	}
	va_list args;
	va_start(args, msg);		// args after 'msg' are unknown
	vfprintf(stderr, msg, args);
	fprintf(stderr,"\n");
	fflush(stderr);
	va_end(args);
}
/**
 * Display warning message if unless quiet_mode is enabled.
 * 
 * @param msg  Format string as described in fprintf()
 */
void warning (const char* msg, ...) {
	if (quiet_mode) {
		return;
	}
	fprintf(stderr,"WARNING: ");
	va_list args;
	va_start(args, msg);		// args after 'msg' are unknown
	vfprintf(stderr, msg, args);
	fprintf(stderr,"\n");
	fflush(stderr);
	va_end(args);
}

/**
 * Signal handler for handling SIGPIPE and...
 */
void signal_handler(int signum, siginfo_t *info, void *ptr) {
	debug (1, "Received signal %d originating from PID %lu\n", signum, (unsigned long)info->si_pid);
	//exit(EXIT_SUCCESS);
	exit_flag = TRUE;
}


int main( int argc, char **argv) {

	libusb_device_handle *zena;

	int channel = -1;			// no default 802.15.4 channel
	int format = FORMAT_PCAP;	// PCAP is default output format
	int scan_mode = FALSE;
	int drop_bad_packets = TRUE;
	int exit_time = -1;

	int c;

	// Setup signal handler. Catching SIGPIPE allows for exit when 
	// piping to Wireshark for live packet feed.
	//signal(SIGPIPE, signal_handler);
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = signal_handler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGPIPE, &act, NULL);


	// Parse command line arguments. See usage() for details.
	while ((c = getopt(argc, argv, "bc:d:f:hqs:t:v")) != -1) {
		switch(c) {
			case 'b':
				drop_bad_packets = FALSE;
				break;
			case 'c':
				channel = atoi (optarg);
				if (channel < 11 || channel > 26) {
					fprintf (stderr, "ERROR: Invalid channel. Must be in rage 11 to 26. Use -h for help.\n");
					exit(-1);
				}
				break;
			case 'd':
				debug_level = atoi (optarg);
				break;
			case 'f':
				if (strcmp(optarg,"pcap")==0) {
					format = FORMAT_PCAP;
				} else if (strcmp(optarg,"usbhex")==0) {
					format = FORMAT_USBHEX;
				} else {
					fprintf(stderr,"ERROR: unrecognized output format '%s'. Only pcap or usbhex allowed.\n",optarg);
					exit(-1);
				}
            	break;
			case 'h':
				version();
				usage();
				exit(EXIT_SUCCESS);
			case 'q':
				quiet_mode = TRUE;
				break;
			case 's':
				scan_mode = TRUE;
				usb_timeout = atoi (optarg);
				break;
			case 't':
				exit_time = atoi(optarg);
				break;
			case 'v':
				version();
				exit(EXIT_SUCCESS);
			case '?':	// case when a command line switch argument is missing
				if (optopt == 'c') {
					fprintf (stderr,"ERROR: 802.15.4 channel 11 to 26 must be specified with -c\n");
					exit(-1);
				}
				if (optopt == 'd') {
					fprintf (stderr,"ERROR: debug level 0 .. 9 must be specified with -d\n");
					exit(-1);
				}
				if (optopt == 'f') {
					fprintf (stderr,"ERROR: pcap or usbhex format must be specified with -f\n");
					exit(-1);
				}
				break;
		}
	}

	if (channel == -1) {
		fprintf (stderr,"ERROR: 802.15.4 channel is mandatory. Specify with -c. Use -h for help.\n");
		exit(EXIT_FAILURE);
	}

	if (debug_level > 0) {
		fprintf (stderr,"DEBUG: debug level %d\n",debug_level);
	}

	// Locate ZENA on the USB bus and get handle.
	if ((zena = setup_libusb_access()) == NULL) {
		fprintf (stderr, "ERROR: ZENA device not found or not accessible\n");
		exit(EXIT_FAILURE);
	}

	// Set 802.15.4 channel
	int status = zena_set_channel (zena,channel);
	if (status < 0) {
		fprintf (stderr, "ERROR: error setting ZENA to 802.15.4 channel %d, errorCode=%d\n",channel,status);
		exit(EXIT_FAILURE);
	} 

	// Write PCAP header
	if (format == FORMAT_PCAP) {
		fwrite(&PCAP_MAGIC, sizeof(int), 1, stdout);    
		fwrite(&PCAP_VERSION_MAJOR, sizeof(short), 1, stdout);
		fwrite(&PCAP_VERSION_MINOR, sizeof(short), 1, stdout);
		fwrite(&PCAP_TZ, sizeof(int), 1, stdout);				// thiszone: GMT to local correction
		fwrite(&PCAP_SIGFIGS, sizeof(int), 1, stdout);			// sigfigs: accuracy of timestamps
		fwrite(&PCAP_SNAPLEN, sizeof(int), 1, stdout);			// snaplen: max len of packets, in octets
		fwrite(&PCAP_LINKTYPE, sizeof(int), 1, stdout);		// data link type
	}

	int i,j,data_len,packet_len,packet_len_plus_2,ts_sec,ts_usec;

	// Allocate buffer for usb_interrupt_read requests
	unsigned char usbbuf[64];
	//unsigned char packetbuf[128];
	
	// Get start time of capture. Won't worry about subsecond resolution for this.
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	int start_sec = tp.tv_sec;

	// Store the number of bytes actually transferred here
	int nbytes;

	// Packet counter
	int npacket=0;

	zena_packet_t zena_packet;

	// Main loop
	while ( ! exit_flag ) {

		// If scan_mode is TRUE, cycle through all the 802.15.4 channels looking
		// for packets. For some reason it seems to be necessary to close the 
		// USB device and libusb library and reopen it for the channel change to 
		// work reliably. Why?

		if (scan_mode) {

			channel++;
			if (channel > 26) {
				channel = 11;
			}

			// It seems to be necessary to reset libusb (close library and 
			// re-initialize it) for zena_set_channel() to be successful.
			debug(9,"Closing ZENA to facilitate 802.15.4 channel change");
			libusb_close (zena);
			debug(9,"Closing libusb library to facilitate 802.15.4 channel change");
			libusb_exit(NULL);
			debug(9,"Reopening ZENA");
		        if ((zena = setup_libusb_access()) == NULL) {
				fprintf (stderr, "ERROR: unable to reopen ZENA during 80.15.4 channel change\n");
				exit(EXIT_FAILURE);
			}
	
			debug (1,"Setting 802.15.4 channel to %d",channel);
			status = zena_set_channel(zena,channel);
			if (status<0) {
				fprintf (stderr,"ERROR: error setting 802.15.4 channel to %d during scan, errorCode=%d\n",channel, status);
				exit(EXIT_FAILURE);
			} 

			// TODO: bug - we can have packet received from the
			// previous 802.15.4 channel in the buffer at this
			// point. When outputted it will be incorrectly
			// tagged with the new channel number. Can we purge
			// the buffer somehow?
			
		}

		switch (format) {

			case FORMAT_USBHEX:

				status = libusb_interrupt_transfer(zena, ZENA_EP_PACKETS, usbbuf, 64, &nbytes, usb_timeout);
				// check for timeout and silently ignore
				if (status == LIBUSB_ERROR_TIMEOUT) {
					debug(9,"libusb_interrupt_transfer(): timeout");
					continue;
				}

				// get host time of packet reception
				clock_gettime(CLOCK_REALTIME, &tp);
				if ( (exit_time>0) && (tp.tv_sec > (start_sec + exit_time))) {
					debug(1,"Exit time reached. Exiting.");
					exit(EXIT_SUCCESS);
				}

				// a real error (ie not timeout)
				if (status < 0) {
					fprintf (stderr,"ERROR: error retrieving ZENA packet, errorCode=%d\n", status);
					continue;
				}

				// Packet timestamp
				fprintf (stdout,"%ld.%ld ",tp.tv_sec,tp.tv_nsec);

				// 802.15.4 channel
				fprintf (stdout, "%02x ", channel);

				// Echo USB 64 byte packet to screen. Each byte as hex separated by space. 
				// One line per packet.
				for (j = 0; j < 64; j++) {
					fprintf (stdout, "%02x ", usbbuf[j] & 0xff);
				}
				fprintf (stdout, "\n");
				fflush (stdout);
				break;

			case FORMAT_PCAP:
				status = zena_get_packet (zena, &zena_packet);
				if (status == LIBUSB_ERROR_TIMEOUT) {
					// A timeout is a normal event. No action.
					break;
				}
				if (status != 0) {
					fprintf (stderr,"ERROR: retrieving packet, errorCode=%d\n",status);
					break;
				}

				// Ensure that zena_packet.packet_len is a sane value. Occasionally getting crazy
				// values which causes segv when accessing the zena_packet.packet[] buffer.
				zena_packet.packet_len &= 0xff;
				if (zena_packet.packet_len > 125) {
					fprintf (stderr,"ERROR: invalid packet length, len=%d\n",zena_packet.packet_len);
					break;
				}

				if (  ( ! zena_packet.fcs_ok) && drop_bad_packets ) {
					warning ("dropping corrupted packet\n");
					break;
				}

				npacket++;

				// Write PCAP packet header
				fwrite (&zena_packet.host_ts_sec, sizeof(int), 1, stdout);	// ts_sec: timestamp seconds
				fwrite (&zena_packet.host_ts_usec, sizeof(int), 1, stdout);	// ts_usec: timestamp microseconds

				// Small problem re FCS. ZENA does not provide this information.
				// Solution is in the case of a good packet not to include FCS
				// and Wireshark will ignore it. In the case were the FCS is 
				// known to be bad, we'll include a deliberatly wrong FCS. For
				// the moment this will be a fixed value (0x0000), but ideally
				// it should be computed from the packet and the +1 to guarantee
				// it is a bad FCS.

				if (zena_packet.fcs_ok) {

					packet_len_plus_2 = zena_packet.packet_len+2;
						
					// write packet excluding FCS
					fwrite (&zena_packet.packet_len, sizeof(int), 1, stdout);
					fwrite (&packet_len_plus_2, sizeof(int), 1, stdout);	// full frame included 2 FCS octets
					fwrite (zena_packet.packet, 1, zena_packet.packet_len, stdout);
				} else {

					// two extra bytes for deliberately wrong FCS
					
					fwrite (&packet_len_plus_2, sizeof(int), 1, stdout);
					fwrite (&packet_len_plus_2, sizeof(int), 1, stdout);
					zena_packet.packet[zena_packet.packet_len] = 0;
					zena_packet.packet[zena_packet.packet_len+1] = 0;
					fwrite (zena_packet.packet, 1, packet_len_plus_2, stdout);
				}

				fflush(stdout);
				break;


		} // end switch


	} // end main loop

	// Release USB interface and close USB connection.
	// This code never reached at the moment -- need to implement signal handler for this.
	// However I've noticed no resource leaks. Process kill seems to take care of this.
	libusb_close (zena);
	libusb_exit(NULL);

	debug (1, "Normal exit");
	return EXIT_SUCCESS; 
}
