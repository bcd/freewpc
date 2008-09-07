
#ifndef _WPCLIB_H
#define _WPCLIB_H

int udp_socket_create (int port);
int udp_socket_send (int s, int dstport, const void *data, int len);
int udp_socket_receive (int s, int dstport, void *data, int len);
int udp_socket_close (int s);

#define NUM_DMD_PHASES 3

struct wpc_message
{
	unsigned char code;
	unsigned int  timestamp;
	unsigned int  len;
	union {
		unsigned char data[1600];
		struct _dmdpage_info
		{
			struct
			{
				unsigned int page;
				unsigned char data[512];
			} phases[NUM_DMD_PHASES];
		} dmdpage;

		struct _dmdvisible_info
		{
			unsigned int phases[NUM_DMD_PHASES];
		} dmdvisible;
	} u;
};

int wpc_msg_init (int code, struct wpc_message *msg);
int wpc_msg_insert (struct wpc_message *msg, const void *p, int len);
int wpc_msg_send (int s, int dstport, struct wpc_message *msg);

#define CODE_DMD_PAGE 0
#define CODE_LAMPS 1
#define CODE_SWITCHES 2
#define CODE_DMD_VISIBLE 3
#define CODE_COILS 4
#define CODE_GEN_ILLUMS 5

#endif
