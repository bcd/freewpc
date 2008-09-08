
#include <stdio.h>
#include <sys/errno.h>
#include "wpclib.h"

int port = 9000;

int main (int argc, char *argv[])
{
	int s = udp_socket_create (port);
	struct wpc_message msg;
	int rc;

	for (;;)
	{
		rc = udp_socket_receive (s, port ^ 1, &msg, sizeof (msg));
		if (rc >= 0)
		{
			printf ("Received code=%02X\n", msg.code);
		}
		else
		{
			usleep (10 * 1000UL);
		}
	}
}

