#include "rio.h"


/**********************************************
 * Name: rio_readn
 * Function: this function is redirectory read,
 *			 without buffer,this function can 
 *			 charge the interrupt and recall 
 *			 read again,this function can make 
 *			 all data which need to read 
 *			 be read.
 * Return: if encounter EOF,only return a lack
 *			of value.
 ********************************************* */

ssize_t rio_readn(int fd, void *usrbuf,size_t n)
{
	size_t nleft = n; //make nleft as unread 
	ssize_t nread;    //mark as read
	char *bufp = usrbuf; 

	/*use a loop, to make sure 
	 *that all need data has been read*/
	while(nleft > 0)
	{
		if((nread = read(fd,bufp,nleft)) < 0)
		{
			if(errno == EINTR)  /*interrupted by sig handler return*/
				nread = 0;      /*and call read() again*/
			else
				return -1;     /*here is real error by read()*/
		}else if(nread == 0)
			break;  /*Mean encounter EOF*/
		nleft -= nread;
		bufp  += nread;
	}
	
	return (n - nleft);   /*return >= 0*/
}

/*****************************************************
 * Name: rio_writen
 * Function: this function used to wirte a buf to file
 *			 unless wirte() is error, it never return a 
 *			 lack of value.
 * Return: if error, return -1, else return n which how many
 *		   bytes have been writen
 *******************************************************/
ssize_t rio_writen(int fd,void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = usrbuf;

	while(nleft > 0)
	{
		if((nwritten = write(fd,bufp,nleft)) < 0)
		{
			if(errno == EINTR)
				nwritten = 0;
			else
				return -1;    /*write() is error*/
		}else if(nwritten == 0)
			break;   /*mean encounter EOF*/
		nleft -= nwritten;
		bufp  += nwritten;

	}

	return n;  /*mean all of data have been writen*/
}


/*********************************************************
 * Name: rio_readinitb
 * Function: this function is used to init a struct rio_t
 * Return:  void
 *********************************************************/
void rio_readinitb(rio_t *rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}


/*************************************************************
 * Name: rio_read
 * Function: this function is used to read n bytes from a file
 *			 or terminal, it save data to a buffer, if the buffer
 *			 is empty, it call read to fill this buffer,then we 
 *			 use memcpy to copy the smaller between n and  rio_cnt
 * Return: return quantity of read
 *****************************************************************/
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
	int cnt;

	/*refill the buffer*/
	while(rp->rio_cnt <= 0)
	{
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf,sizeof(rp->rio_buf));
		if(rp->rio_cnt < 0)
		{
			if(errno !=	EINTR)
			{
				return -1;
			}
		}else if(rp->rio_cnt == 0)
			return 0;   /*EOF*/
		else
			rp->rio_bufptr = rp->rio_buf;
	}

	cnt = n;
	if(rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf,rp->rio_bufptr,cnt);
	rp->rio_cnt -= cnt;
	rp->rio_bufptr += cnt;

	return cnt;
}




/**************************************************************
 * Name: rio_readlineb
 * Function: read a line from the file and use supply a buffer
 *			 this function is call rio_read,which defined above
 * Return:  return quantity of read.
 *************************************************************/

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	int n, rc;
	char c, *bufp = usrbuf;

	for(n = 1; n < maxlen; n++)
	{
		if((rc = rio_read(rp, &c, 1)) == 1)
		{
			*bufp++ = c;
			if(c == '\n')
				break;
		}else if(rc == 0)
		{
			if(n == 1)
				return 0;   /*EOF, no data read*/
			else
				break;     /*EOF, data was read*/
		}else
			return -1;  /*Error*/
			
	}

	*bufp = 0;
	return n;
}

/*************************************************************
 * Name:rio_readnb
 * Function: this function is a advance version, it supply a buffer
 *			 
 * Return: return the quantity of read
 ***************************************************************/

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while(nleft > 0)
	{
		if((nread = rio_read(rp,bufp,nleft)) < 0)
		{
			if(errno == EINTR)
				nread = 0;   /*use for recall this  rio_read()*/
			else
				return -1;  /*error*/
		}else if(nread == 0)
			break;			/*EOF*/
		nleft -= nread;
		bufp  += nread;
	}

	return (n - nleft);
}


