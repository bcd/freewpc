
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

/** Turn on/off keybuffering.  Pass a zero to put the
console in raw mode, so keystrokes are not echoed.
Pass nonzero flag to go back to the default mode. */
int posix_input_config (int fd, int buffering)
{
   struct termios tio;
	char buf;
   tcgetattr (fd, &tio);
   if (!buffering) /* 0 = no buffering = not default */
	{
      tio.c_lflag &= ~ICANON;
		tio.c_cc[VTIME] = 0;
		tio.c_cc[VMIN] = 0;
	}
   else /* 1 = buffering = default */
      tio.c_lflag |= ICANON;
   tcsetattr (fd, TCSANOW, &tio);
	return 0;
}


int posix_input_poll (int fd)
{
	char buf;
	ssize_t res = read (fd, &buf, 1);
	if (res <= 0)
		return res;
	return buf;
}

