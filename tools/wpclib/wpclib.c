#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include "wpclib.h"

#define UDP_PORT 7400

#define CLIENT_PORT_OFFSET 0
#define SERVER_PORT_OFFSET 1

int client_port = UDP_PORT + CLIENT_PORT_OFFSET;
int server_port = UDP_PORT + SERVER_PORT_OFFSET;

void udp_socket_error (void)
{
	abort ();
}

int udp_socket_create (int port)
{
	int rc;
	int s;
	struct sockaddr_in myaddr;

	s = socket (PF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		fprintf (stderr, "could not open socket, errno=%d\n", errno);
		udp_socket_error ();
		return s;
	}

	rc = fcntl (s, F_SETFL, O_NONBLOCK);
	if (rc < 0)
	{
		fprintf (stderr, "could not set nonblocking, errno=%d\n", errno);
		udp_socket_error ();
		return rc;
	}

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons (port);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	rc = bind (s, (struct sockaddr *)&myaddr, sizeof (myaddr));
	if (rc < 0)
	{
		fprintf (stderr, "could not bind socket, errno=%d\n", errno);
		udp_socket_error ();
		return rc;
	}

	return s;
}

int udp_socket_send (int s, int dstport, const void *data, int len)
{
	int rc;
	struct sockaddr_in to;

	to.sin_family = AF_INET;
	to.sin_port = htons (dstport);
	to.sin_addr.s_addr = inet_addr ("127.0.0.1");
	rc = sendto (s, data, len, 0, (struct sockaddr *)&to, sizeof (to));
	if ((rc < 0) && (errno != EAGAIN))
	{
		fprintf (stderr, "could not send, errno=%d\n", errno);
		udp_socket_error ();
	}
	return rc;
}

int udp_socket_receive (int s, int dstport, void *data, int len)
{
	int rc;
	struct sockaddr_in from;
	int fromlen;

	rc = recvfrom (s, data, len, 0, (struct sockaddr *)&from, &len);
	if ((rc < 0) && (errno != EAGAIN))
	{
		fprintf (stderr, "could not receive, errno=%d\n", errno);
		udp_socket_error ();
	}
	return rc;
}

int wpc_msg_init (int code, struct wpc_message *msg)
{
	msg->code = code;
	msg->timestamp = 0;
	msg->len = 0;
	return 0;
}

int wpc_msg_insert (struct wpc_message *msg, const void *p, int len)
{
	memcpy (msg->u.data + msg->len, p, len);
	msg->len += len;
}

int wpc_msg_send (int s, int dstport, struct wpc_message *msg)
{
	int rc = udp_socket_send (s, dstport, msg, msg->len + sizeof (struct wpc_message) - 1000);
	if (rc < 0)
	{
		fprintf (stderr, "error: could not send!\n");
	}
	return rc;
}

int udp_socket_close (int s)
{
	close (s);
	return 0;
}


#ifdef STANDALONE
int main (int argc, char *argv[])
{
	int server, client;
	char sendbuf[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	char recvbuf[8];

	server = udp_socket_create (server_port);
	client = udp_socket_create (client_port);
	printf ("Server = %d, Client = %d\n", server, client);

	getchar ();
	printf ("Sending data\n");
	udp_socket_send (client, server_port, sendbuf, sizeof (sendbuf));
	printf ("Receiving data\n");
	udp_socket_receive (server, client_port, recvbuf, sizeof (recvbuf));

	if (memcmp (sendbuf, recvbuf, sizeof (sendbuf)))
	{
		printf ("Buffers differ.\n");
	}

	udp_socket_close (server);
	udp_socket_close (client);
}
#endif
