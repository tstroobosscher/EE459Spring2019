/*
 *	USC EE459 Spring 2019 Team 17 - OBD2 Test code
 */

/*
 *	February 26, 2019
 *		So i need to write the routine that is going to grab the data bytes 
 *		from the OBD2 response efficiently. All responses are preambled with
 *		0x40 or'ed with the command number (we saw something extremely 
 *		similar with the SD data-link layer code). All bytes before that
 *		preamble are ignored, and each command has an expected number og bytes
 *		to listen for
 *		
 *		Also i think its important to keep layering in mind. we probably 
 *		shouldnt be doing string comparison or byte filtering at all at the
 *		lowest level. we just need to copy the bytes into a buffer of an
 *		appropriate size, and let the upper layers deal with the filtering
 */

/*
 *	Wow, so declaring variables after string buffers can cause issues when
 *	the strings are expected to be NULL terminated, switch the order of i 
 *	and buf and the program will run incorrectly since printf is looking
 *	for a NULL char (when using printf())
 */

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_PARITY 0

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

#define BUF_SIZE 64
#define MAX_TRIALS 64
#define HEX_BASE 16
#define PREAM_CONST 0x40
#define OBD_CMD_LEN 4 + 1 /* +1 for carriage return */

const char *NO_DATA = "NO DATA";
const char *OK = "OK";
const char *NO_CONNECT = "UNABLE TO CONNECT";

const char *ELM_RESET = "ATZ\r";
const char *ECHO_OFF = "ATE0\r";
const char *SEARCH_BUS = "ATSP0\r";
const char *GET_DEVS = "0100\r";

int set_tty_attr(int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr (fd, &tty) != 0) {
            printf("error %d from tcgetattr", errno);
            return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
            printf("error %d from tcsetattr", errno);
            return -1;
    }
    return 0;
}

void set_tty_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0) {
            printf("error %d from tggetattr", errno);
            return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
            printf("error %d setting term attributes", errno);
}

int elm_response_complete(const char *buf) {
	/*
	 *	console will send a '>' once it is ready for the next command
	 */

	if(strstr(buf, ">"))
		return 1;

	return 0;
}

int elm_write(int device, const char *buf, int len) {
	/*
	 *	write n bytes from the buffer
	 */

	int ret =  0;

	while(len > 0) {

		ret = write(device, buf, len);
		if(ret < 0)
			return -1;

		len -= ret;
	}

	/* why was this here ? */
	//write(device, "\r", 1);

	return 0;

}

int elm_read(int device, char *buf, int len) {
	/*
	 *	read n bytes and put into the buffer
	 */

	char *cur = buf;
	int ret = 0;
	int n;

	while(len) {
		n = read(device, cur, 1);

		cur += n;
		ret += n;
		len -= n;

		*cur = '\0';

		if(elm_response_complete(buf))
			break;

	}

	return 0;
}

int elm_command(int device, const char *cmd, int len, char *buf, int size) {
	/*
	 *	send the command and then get the raw bytes until the '>', or tout
	 */

	if(elm_write(device, cmd, len) < 0)
		return -1;

	return (elm_read(device, buf, size));

}

char* obd_resp(const char *cmd, char *buf) {
	/*
	 *	find the beginning of the response data
	 */

	char pream[3];

	if((strlen(cmd) != 5))
		return NULL;

	if(!strncpy(pream, cmd, 2))
		return NULL;

	/* NULL terminate substr */
	pream[2] = '\0';

	return strstr(pream, buf);
}

int obd_command(int device, const char *cmd, char *buf, int size) {
	/*
	 *	send cmd of size bytes to device, expect ret bytes and put into buf
	 *	
	 *	cmds are NULL terminated
	 *	
	 *	cmds are 4 digit hex values passed as ascii
	 *	
	 *	return a pointer to the preamble of the return argument with a NULL
	 *	char at the end of the hex sequence.
	 */

	/* 
	 *	string ops are just easier and more efficient with dynamic allocation.
	 *	but we really shouldnt need dynamic allocation. all the bytes are held
	 *	in a butter right off the bat, and all the operations we are doing 
	 *	are monotonically increasing through the index. hmmm
	 */

	char *data;
	int n = 0;

	printf("obd_command: %s\n", cmd);

	if((strlen(cmd) != OBD_CMD_LEN)) {
		printf("obd_command: incorrect command format\n");
		return -1;
	}

	if(elm_command(device, cmd, OBD_CMD_LEN, buf, size) < 0) {
		printf("obd_command: elm device failure\n");
		return -1;
	}

	if(strstr(buf, NO_DATA) || strstr(buf, NO_CONNECT)) {
		printf("obd_command: OBD not configured\n");
		return -1;
	}
	
	if(!(data = obd_resp(cmd, buf))) {
		printf("obd_command: unable to find responses\n");
		return -1;
	}

	for(int i = 0; i < strlen(data); i++) {
		if(isspace(data[i]))
			continue;
		else if(isxdigit(data[i])) {
			data[n++] = data[i];
		}
	}

	data[n] = '\0';

	printf("%s\n", data);
	// strtol!

	return 0;
}

int initialize_elm(int device) {

	char buf[BUF_SIZE];

	if(elm_command(device, ELM_RESET, sizeof(ELM_RESET), buf, sizeof(buf)) < 0)
		return -1;

	printf("ATZ: %s\n", buf);

	if(elm_command(device, ECHO_OFF, sizeof(ECHO_OFF), buf, sizeof(buf)) < 0)
		if(!strstr(buf, OK))
			return -1;

	printf("ATE0: %s\n", buf);

	if(elm_command(device, SEARCH_BUS, sizeof(SEARCH_BUS), buf, 
		sizeof(buf)) < 0)
		if(!strstr(buf, OK))
			return -1;

	printf("ATSP0: %s\n", buf);


	return 0;
}

int initialize_obd(int device) {

	char buf[BUF_SIZE];

	if(obd_command(device, GET_DEVS, buf, sizeof(buf)) < 0)
		return -1;

	return 0;
}

char buf[BUF_SIZE];

int main(int argc, char * argv[]) {

	const char *pname = argv[0];

	if(argc != 2) {
		printf("%s: incorrect number of args\n", pname);
		return -1;
	}

	const char *port = argv[1];

	int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0) {
		printf("%s: error openning tty\n", pname);
		return -1;
	}

	if(set_tty_attr(fd, B38400, NO_PARITY) < 0) {
		printf("%s: error setting tty attributes\n", pname);
		return -1;
	}

	set_tty_blocking(fd, 0);
	if(initialize_elm(fd) < 0)
		printf("initialize_elm: error\n");

	if(initialize_obd(fd) < 0)
		printf("initialize_obd: error\n");

	return 0;
}

