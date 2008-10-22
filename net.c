#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "list.h"

#define NET_MAXDATASIZE 4096

#define NET_DISCONNECTED 1
#define NET_CONNECTED 0
#define NET_ERROR -1

#define NET_ERR_MEM -1
#define NET_ERR_CONNECTED -20
#define NET_ERR_DISCONNECTED -30
#define NET_ERR_BAD_ARG -2
#define NET_ERR_SEND -3
#define NET_ERR_RECV -4
#define NET_ERR_CONNECT -5
#define NET_ERR_GETHOSTBYNAME -6
#define NET_ERR_SOCKET -7

#define NET_TCP 1
#define NET_UDP 2

typedef struct  {
	node_l *in;
	node_l *out;
	int sockfd;
	int rcvd;
	int sent;
	int status;
	int type;
} net;

typedef struct {
	void *data;
	size_t len;
	size_t offset;
} chunk;




static void list_reverse(node_l **list)
{
	node_l *new = NULL, *node;

	if(list_is_empty(list))
		return;

	while((node = list_pop_first_node(list)) != NULL) {
		list_prepend_node(&new, node);
	}

	*list = new;
}

static chunk *chunk_alloc()
{
	return(malloc(sizeof(chunk)));
}

static void chunk_free(chunk *c)
{
	if(c == NULL)
		return;

	if(c->data != NULL)
		free(c->data);

	free(c);
}

static int chunk_set(chunk *c, void *data, size_t len)
{
	if(c == NULL)
		return(NET_ERR_BAD_ARG);

	if(data == NULL)
		return(NET_ERR_BAD_ARG);

	if((c->data = malloc(len)) == NULL)
		return(NET_ERR_MEM);

	memcpy(c->data, data, len);
	c->len = len;
	c->offset = 0;

	return(0);
}

static chunk *chunk_create(void *data, size_t len)
{
	chunk *c;

	if(data == NULL)
		return(NULL);

	c = chunk_alloc();

	if(c == NULL)
		return(NULL);

	if(chunk_set(c, data, len) < 0) {
		chunk_free(c);
		return(NULL);
	}

	return(c);
}

static int net_init(net *n)
{
	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	n->in = NULL;
	n->out = NULL;
	n->sockfd = 0;
	n->sent = 0;
	n->rcvd = 0;
	n->status = NET_DISCONNECTED;
	n->type = -1;  /* FIXME */

	return(0);
}

net *net_alloc()
{
	net *n;

	n = malloc(sizeof(net));

	if(n == NULL)
		return(NULL);

	net_init(n);

	return(n);
}

void net_free(net *n)
{
	free(n);
}

int net_connect(net *n, const char *host, unsigned short int port)
{
	struct hostent *he;
	struct sockaddr_in remote_addr;

	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	if ((he=gethostbyname(host)) == NULL) {
		return(NET_ERR_GETHOSTBYNAME);
	}

	if ((n->sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
	}

	remote_addr.sin_family = AF_INET;    // host byte order
	remote_addr.sin_port = htons(port);  // short, network byte order
	remote_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(remote_addr.sin_zero, '\0', sizeof remote_addr.sin_zero);

	if (connect(n->sockfd, (struct sockaddr *)&remote_addr,
	                        sizeof remote_addr) == -1) {
		return(NET_ERR_CONNECT);
	}

	n->status = NET_CONNECTED;
	return(NET_CONNECTED);
}

int net_disconnect(net *n)
{
	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	close(n->sockfd);

	n->status = NET_DISCONNECTED;

	return(0);
}

int net_send(net *n)
{
	chunk *c;
	ssize_t num;
	node_l *node;

	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	if(n->status != NET_CONNECTED)
		return(NET_ERR_DISCONNECTED);

	c = (chunk *)list_get_last(&(n->out));

	num = send(n->sockfd, c->data + c->offset, c->len - c->offset, 0);

/*	if(num == 0)
		net_disconnect(n);

	FIXME
*/

	if(num > 0) {
		c->offset += num;
		n->sent += num;
	}

	if(c->offset == c->len) {
		node = list_pop_last_node(&(n->out));
		chunk_free(node->data);
		list_free_node(node);
	}

	return(num);
}

int net_recv(net *n)
{
	char buf[NET_MAXDATASIZE];
	int num = 0;
	chunk *c;

	errno = 0;
	num = recv(n->sockfd, buf, NET_MAXDATASIZE - 1, 0);
	buf[num] = '\0';

	if(errno == EAGAIN)
		return(0);

	if(num < 0)
		return(num);

	if(num == 0) {
		net_disconnect(n);
		return(num);
	}

	c = chunk_create(buf, num+1);
	if(c == NULL)
		return(NET_ERR_MEM);

	printf("%d\n", num);

	if(list_prepend(&(n->in), (void *)c) < 0) {
		chunk_free(c);
		return(NET_ERR_MEM);
	}

	n->rcvd += num;

	return(num);
}

int net_queue_data(net *n, void *data, size_t len)
{
	chunk *c;

	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	if(data == NULL)
		return(0);

	if(len < 1)
		return(0);

	c = chunk_create(data, len);
	if(c == NULL)
		return(NET_ERR_MEM);

	return(list_prepend(&(n->out), (void *)c));
}

int net_queue_string(net *n, const char *msg)
{
	size_t len;

	if(n == NULL)
		return(NET_ERR_BAD_ARG);

	if(msg == NULL)
		return(0);

	len = strlen(msg);
	if(len == 0)
		return(0);

	return(net_queue_data(n, (void *)msg, len));
}

static void chunk_print(void *c)
{
	chunk *ch = (chunk *)c;

	printf("%s", (char *)(ch->data));
}

static void net_print(net *n)
{
	list_reverse(&(n->in));
	list_foreach(&(n->in), chunk_print);
	list_reverse(&(n->in));
//	list_foreach(&(n->out), chunk_print);
}

static void net_queue_size(net *n)
{
	printf("len (in):  %d\n", list_size(&(n->in)));
	printf("len (out): %d\n", list_size(&(n->out)));
}

int main(int argc, char *argv[])
{
	net *n;
	node_l *tmp;
	chunk *c;
	char *msg = "GET /search?hl=de&q=%22meine+oma+f%C3%A4hrt+im+h%C3%BChnerstall+motorrad%22&btnG=Suche&meta= HTTP/1.0\n\n";
	//char *msg = "GET /index.php HTTP/1.0\n\n";


	if(argc != 4) {
		printf("USAGE: %s <host> <port> <msg>\n", argv[0]);
		exit(-1);
	}

	if((n = net_alloc()) == NULL) {
		fprintf(stderr, "Huh?\n");
		exit(-1);
	}


	//net_queue_string(n, argv[3]);
	net_queue_string(n, msg);
	net_connect(n, argv[1], atoi(argv[2]));
	net_print(n);

	net_send(n);
//	sleep(1);
	while(net_recv(n) >= 0) {
		if(n->status == NET_DISCONNECTED) break;
	}

	net_print(n);

	printf("\n\nstatus: %d\n", n->status);
	printf("sent:   %d\n", n->sent);
	printf("rcvd:   %d\n", n->rcvd);

	return(0);
}

