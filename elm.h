/*
 *	USC EE459 Spring 2019 Team 17 - ELM327 Header
 */

#ifndef ELM_H
#define ELM_H

#define BUF_SIZE 64

int initialize_elm(int device);
int elm_write(uint8_t device, const char *buf);
int elm_command(int device, const char *cmd, int len, char *buf, int size);

#endif
