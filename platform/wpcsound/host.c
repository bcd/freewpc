
#include <freewpc.h>
#include <queue.h>

#define HOST_BUFFER_SIZE 16

/* Queues the data from the sound board to the CPU board */
struct {
	U8 head;
	U8 tail;
	U8 elems[HOST_BUFFER_SIZE];
} host_write_queue;


/* Queues the data from the CPU board to the sound board */
struct {
	U8 head;
	U8 tail;
	U8 elems[HOST_BUFFER_SIZE];
} host_read_queue;


/** Receive a character from the host, and queue it for
later processing. */
void host_receive (void)
{
}

/** Send a pending character back to the host. */
void host_send (void)
{
}

void host_init (void)
{
	disable_firq ();
	queue_init ((queue_t *)&host_write_queue);
	queue_init ((queue_t *)&host_read_queue);
	enable_firq ();
}

