#ifndef _RIO_H_
#define _RIO_H_

#define RIO_BUFSIZE 8192
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
	int rio_fd;                /*descriptor for this internal buf*/
	int rio_cnt;			   /*Uread bytes in internal buf*/
	char *rio_bufptr;          /*next unread byte in internal buf*/
	char rio_buf[RIO_BUFSIZE]; /*internal buffer*/
}rio_t;


ssize_t rio_readn(int fd, void *usrbuf,size_t n);
ssize_t rio_writen(int fd, void *usrbuf,size_t n);

void rio_readinitb(rio_t *rp,int fd);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf,size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf,size_t n);

#endif

