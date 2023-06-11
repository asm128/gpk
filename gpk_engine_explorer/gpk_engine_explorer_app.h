#include "gpk_engine_explorer.h"

#include "gpk_framework.h"

#define DISABLE_D3D11
//#define DISABLE_XAUDIO2
#ifndef DISABLE_XAUDIO2
#	include "audio.h"
#endif

#ifndef GPK_ENGINE_EXPLORER_APP_H
#define GPK_ENGINE_EXPLORER_APP_H

struct SApplication {
	::gpk::SFramework	Framework;

	::gpk::SEditor		Editor;

#ifndef DISABLE_D3D11
	::SD3DApplication	D3DApp;
#endif
#ifndef DISABLE_XAUDIO2
	AUDIO_STATE			AudioState;
#endif
						SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept : Framework(runtimeValues) {}
};

#endif // GPK_ENGINE_EXPLORER_APP_H
