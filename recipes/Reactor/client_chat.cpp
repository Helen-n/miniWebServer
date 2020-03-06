#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff
#endif	/* INADDR_NONE */

#define	BUFSIZE 	64
#define	LINELEN		128
#define	UNIXEPOCH	2208988800UL	/* UNIX epoch, in UCT secs	*/
#define	MSG		"what time is it?\n"

int	connectsock(const char *host, const char *service,const char *transport);
int	connectUDP(const char *host, const char *service);
int connectTCP(const char *host, const char *service );

int getUsername(void);
int getTime(char *time_char);
int	tranTime(time_t time1,char *Time);
int send_msg(int fd);
int	TCP_chat(const char *host, const char *service);

int	errexit(const char *format, ...);

char user_name[10];  //用户名

int main(int argc,char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "echo";	/* default service name		*/

	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		service = argv[2];
		/* FALL THROUGH */
	case 2:
		host = argv[1];
		break;
	default:
		fprintf(stderr, "usage: TCPecho [host [port]]\n");
		exit(1);
	}

	TCP_chat(host, service);

	exit(0);
}

int TCP_chat(const char *host, const char *service)
{
	pthread_t th;
	pthread_attr_t ta;


	char	buf[LINELEN+1];		/* buffer for one line of text	*/
	int	s, n;			/* socket descriptor, read count*/
	int	outchars, inchars;	/* characters sent and received	*/

	while(getUsername() == 1)  //获取用户名
	{
		;
	}

	(void)pthread_attr_init(&ta);
	(void)pthread_attr_setdetachstate(&ta,PTHREAD_CREATE_DETACHED);

	s = connectTCP(host, service);

	if(pthread_create(&th,&ta,(void * (*)(void *))send_msg,(void *)s) < 0)
		errexit("pthread_create:%s\n",strerror(errno));

	/* 显示 */
	while (1) {

		memset(&buf,0,sizeof(buf));  //清空buf
		n=read(s,buf,sizeof(buf));
		if(n>0){
		printf("\n");
		fputs(buf, stdout);
		}
	}

	close(s);

	return 0;
}

int send_msg(int fd)
{
	char 	buf[LINELEN+1];
	char 	sendbuf[LINELEN+1+10];
	char 	time_buf[25];


	while(1)
	{
		memset(time_buf,0,sizeof(time_buf));  //清空time_buf

		strcpy(sendbuf,"User:");
		strcat(sendbuf,user_name);

		(void)getTime(time_buf);  //获取时间

		strcat(sendbuf,time_buf);
		strcat(sendbuf,"\n");

		fgets(buf,sizeof(buf),stdin);  //输入信息

		strcat(sendbuf,buf);
		strcat(sendbuf,"\n");

		write(fd,sendbuf,strlen(sendbuf));
	}

	return 0;
}

int getUsername()
{
	printf("------------------------------------\n");
	printf("       EasyChat        v1.1         \n");
	printf("------------------------------------\n");
	printf("Your Username: (10 words)\n");

	fgets(user_name,sizeof(user_name),stdin);  //输入用户名

	if(strlen(user_name)<0){
		memset(user_name,0,sizeof(user_name));
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------------------
 *   getTime - get time from Time server
 *------------------------------------------------------------------------
 */
int getTime(char *time_char)
{

	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*service = "time";	/* default service name		*/
	time_t	now;			/* 32-bit integer to hold time	*/
	int	s, n;			/* socket descriptor, read count*/

	s = connectUDP(host, service);

	(void) write(s, MSG, strlen(MSG));

	n = read(s, (char *)&now, sizeof(now));

	if (n < 0)
		errexit("read failed: %s\n", strerror(errno));

	now = ntohl((unsigned long)now);	/* put in host order	*/
	now -= UNIXEPOCH;		/* convert UCT to UNIX epoch	*/

	(void)tranTime(now,time_char);
	close(s);

	return 0;

}

int tranTime(time_t time,char *Time)
{
	struct tm tm1;
	localtime_r(&time,&tm1);
	 sprintf(Time, "%2.2d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d",
           tm1.tm_year+1900-2000, tm1.tm_mon+1, tm1.tm_mday,
             tm1.tm_hour, tm1.tm_min,tm1.tm_sec);
	return 0;
}

int connectsock(const char *host, const char *service, const char *transport )

{
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct servent	*pse;	/* pointer to service information entry	*/
	struct protoent *ppe;	/* pointer to protocol information entry*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, type;	/* socket descriptor and socket type	*/


	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

    /* Map service name to port number */
	if ( pse = getservbyname(service, transport) )
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port=htons((unsigned short)atoi(service))) == 0)
		errexit("can't get \"%s\" service entry\n", service);

    /* Map host name to IP address, allowing for dotted decimal */
	if ( phe = gethostbyname(host) )
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		errexit("can't get \"%s\" host entry\n", host);

    /* Map transport protocol name to protocol number */
	if ( (ppe = getprotobyname(transport)) == 0)
		errexit("can't get \"%s\" protocol entry\n", transport);

    /* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

    /* Allocate a socket */
	s = socket(PF_INET, type, ppe->p_proto);
	if (s < 0)
		errexit("can't create socket: %s\n", strerror(errno));

    /* Connect the socket */
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		errexit("can't connect to %s.%s: %s\n", host, service,
			strerror(errno));
	return s;
}


int connectTCP(const char *host, const char *service )
{
	return connectsock( host, service, "tcp");
}

int connectUDP(const char *host, const char *service )
{

	return connectsock(host, service, "udp");

}

int errexit(const char *format, ...)
{
	va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}
