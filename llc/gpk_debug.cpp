#include "llc_debug.h"
#ifdef LLC_WINDOWS
#	include <crtdbg.h>
#endif

sttc	int	DebugBreakEnabled	= 0;

int	llc::debugBreakEnable	(int enabled)	nxpt { return DebugBreakEnabled = enabled; }
int	llc::debugBreakDisable	()				nxpt { return debugBreakEnable(false); }
int	llc::debugBreakEnabled	()				nxpt { return DebugBreakEnabled; }
int	llc::debugBreak			()				nxpt {
	if(debugBreakEnabled()) {
#ifdef LLC_WINDOWS
		_CrtDbgBreak();
#endif
	}
	return 0;
}

