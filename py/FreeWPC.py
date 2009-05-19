
import os

class FreeWPC:
	# Root says where the root of the FreeWPC build tree is.
	Root = os.environ['HOME'] + 'src/git/freewpc'

	# Machine says what machine is currently selected.
	MACHINE = 'tz'

	# Native is True for native mode builds
	Native = False
