#include "gpk_engine.h"
#include "gpk_framework.h"

//#define DISABLE_D3D11
#define DISABLE_XAUDIO2

#ifndef DISABLE_D3D11
#	include "gpk_engine_d3d_app.h"
#endif

#ifndef DISABLE_XAUDIO2
#	include "gpk_xaudio2.h"
#endif


#ifndef TEST_GPK_ENTITY_H_23701
#define TEST_GPK_ENTITY_H_23701

struct SApplication {
	::gpk::SFramework		Framework	;
#ifndef DISABLE_D3D11
	::gpk::SD3DApplication	D3DApp			= {};
#endif
#ifndef DISABLE_XAUDIO2
	::gpk::AUDIO_STATE		AudioState		= {};
#endif

	::gpk::SEngine			Engine			= {};

							SApplication	(::gpk::SRuntimeValues & runtimeValues)	noexcept	
	: Framework(runtimeValues) { 
		if(!Framework.GUI) 
			Framework.GUI.create(); 
		Engine.Scene.create();
	}
};

#endif // TEST_GPK_ENTITY_H_23701
