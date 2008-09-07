
#include <stdio.h>
#include <stdlib.h>
#include "wpclib.h"

int port = 9001;

int try_receive (int s)
{
	unsigned char buf[1024];
	struct wpc_message *msg = (struct wpc_message *)buf;
	int n;
	int rc = udp_socket_receive (s, port ^ 1, buf, 1024);

	if (rc >= 0)
	{
		printf ("Code: %02X\n", msg->code);
		printf ("Timestamp: %d\n", msg->timestamp);
		printf ("Length: %d\n", msg->len);

		switch (msg->code)
		{
			case CODE_DMD_PAGE:
				break;

			case CODE_DMD_ACTIVE_PAGE:
				n = *((int *)msg->data);
				printf ("Active page = %d\n", n);
				break;
		}
	}
}


int main (int argc, char *argv[])
{
	int s = udp_socket_create (port);
	struct wpc_message msg;

	for (;;)
	{
		try_receive (s);
		usleep (50 * 1000UL);
#if 0
		getchar ();
		msg.code = 1;
		msg.len = 0;
		udp_socket_send (s, port ^ 1, &msg, sizeof (msg));
#endif
	}
}

