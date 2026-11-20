#include "gpk_debug.h"
#ifdef GPK_WINDOWS
#	include <crtdbg.h>
#endif

sttc	int	DebugBreakEnabled	= 0;

int	gpk::debugBreakEnable	(int enabled)	nxpt { return DebugBreakEnabled = enabled; }
int	gpk::debugBreakDisable	()				nxpt { return debugBreakEnable(false); }
int	gpk::debugBreakEnabled	()				nxpt { return DebugBreakEnabled; }
int	gpk::debugBreak			()				nxpt {
	if(debugBreakEnabled()) {
#ifdef GPK_WINDOWS
		_CrtDbgBreak();
#endif
	}
	return 0;
}

