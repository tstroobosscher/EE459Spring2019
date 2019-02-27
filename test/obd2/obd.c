#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define NO_PARITY 0

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

	write (fd, "ATZ\r\n", 5);

	/*
	 *	Wow, so declaring variables after string buffers can cause issues when
	 *	the strings are expected to be NULL terminated, switch the order of i 
	 *	and buf and the program will run incorrectly since printf is looking
	 *	for a NULL char (when using printf())
	 */

	char buf;
	int i = 0;

	do {
		read(fd, &buf, sizeof(buf));
		write(STDOUT_FILENO, &buf, sizeof(buf));

		i++;
	} while(buf != '>');

	printf("\n\rbytes received: %d\n", i);

	write (fd, "ATE0\r\n", 5);

	i = 0;

	char ret[64];

	for(i = 0; i < 50; i++){
		read(fd, &buf, sizeof(buf));
		write(STDOUT_FILENO, &buf, sizeof(buf));

		ret[i] = buf;

		if(buf == '>')
			break;
	}

	printf("\n\n");

	/* if "OK" not in the return string, error */
	if(!strstr(ret, "OK"))
		printf("%s: ERROR: unable to initialize ELM327\n", pname);
	else
		printf("%s: ELM327 initialized\n", pname);

	write (fd, "0100\r\n", 5);

	i = 0;

	for(i = 0; i < 50; i++){
		read(fd, &buf, sizeof(buf));
		write(STDOUT_FILENO, &buf, sizeof(buf));

		ret[i] = buf;

		if(buf == '>')
			break;
	}

	printf("%s: ret: %s\n", pname, ret);

	/* read battery voltage */
	//ATRV
	
	/* search for protocol */
	//ATSP0



	return 0;
}

