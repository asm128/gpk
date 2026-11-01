#include "gpk_complus.h"

#ifdef GPK_WINDOWS

#include "gpk_n3.h"
#include "gpk_array_ptr.h"

#define WIN32_LEAN_AND_MEAN
#include "XAudio2Versions.h"

#include <wrl/client.h>

#ifndef USING_XAUDIO2_7_DIRECTX
#	pragma comment(lib,"runtimeobject.lib")
#	include <Windows.Devices.Enumeration.h>
#	include <wrl.h>
#	include <ppltasks.h>
#endif

#ifdef USING_XAUDIO2_REDIST
#	include <mmdeviceapi.h>
#	include <Functiondiscoverykeys_devpkey.h>
#endif

#ifndef GPK_XAUDIO2_H_23627
#define GPK_XAUDIO2_H_23627

namespace gpk
{
	stxp u0_t		INPUTCHANNELS	= 1;// number of source channels
	stxp u0_t		OUTPUTCHANNELS	= 8;// maximum number of destination channels supported in this sample
	stxp u0_t		NUM_PRESETS		= 30;

	// Constants to define our world space
	stxp s2_t		XMIN			= -10;
	stxp s2_t		XMAX			= 10;
	stxp s2_t		ZMIN			= -10;
	stxp s2_t		ZMAX			= 10;

	struct AudioDevice {
		std::wstring	Id;
		std::wstring	Description;
	};

	struct SSoundState {
		::gpk::pobj<::gpk::au0_t>	WaveData;
		IXAudio2SourceVoice		* SourceVoice			= 0;

		X3DAUDIO_EMITTER		Emitter					= {};
		X3DAUDIO_CONE			EmitterCone				= {};
		::gpk::n3f2_t			EmitterPos				= {};
		f2_t					EmitterAzimuths			[INPUTCHANNELS]                     = {};
		f2_t					MatrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};

		u2_t				ChannelMask				= 0;
		u2_t				nChannels				= 0;
		X3DAUDIO_DSP_SETTINGS	dspSettings				= {};
		bool					Playing					= false;

		HRESULT					PrepareAudio			(::gpk::view<sc_c> & wavname, IXAudio2*	pXAudio2, IXAudio2MasteringVoice * pMasterVoice, IXAudio2SubmixVoice * pSubmixVoice);
	};

	// Struct to hold audio game state
	struct AUDIO_STATE {
		bool					bInitialized			= false;

		// XAudio2
#ifdef USING_XAUDIO2_7_DIRECTX
		HMODULE					mXAudioDLL;
#endif
		::gpk::pcom<IXAudio2>	XAudio2					= {};

		IXAudio2MasteringVoice	* pMasterVoice			= 0;
		IXAudio2SubmixVoice		* pSubmixVoice			= 0;

		::gpk::pcom<IUnknown>	pVolumeLimiter;
		::gpk::pcom<IUnknown>	pReverbEffect;
		// 3D
		X3DAUDIO_HANDLE			x3DInstance				= {};
		int						nFrameToApply3DAudio	= 0;

		::gpk::au0_t			WaveData;
		IXAudio2SourceVoice		* SourceVoice			= 0;


		u2_t				ChannelMask				= 0;
		u2_t				nChannels				= 0;
		X3DAUDIO_DSP_SETTINGS	dspSettings				= {};

		X3DAUDIO_LISTENER		listener				= {};
		X3DAUDIO_EMITTER		emitter					= {};
		X3DAUDIO_CONE			emitterCone				= {};

		::gpk::n3f2_t			vListenerPos			= {};
		::gpk::n3f2_t			vEmitterPos				= {};
		f2_t					fListenerAngle			= (f2_t)(::gpk::math_pi_2 * .5);
		bool					fUseListenerCone		= true;
		bool					fUseInnerRadius			= true;
		bool					fUseRedirectToLFE		= 0;
		bool					Playing					= false;
		f2_t					FrequencyRatio			= 1.0f;
#ifdef USING_XAUDIO2_7_DIRECTX
		u2_t				SampleRateOriginal		= 0;
		u2_t				SampleRateCurrent		= 0;
#endif

		FLOAT32					emitterAzimuths			[INPUTCHANNELS]                     = {};
		FLOAT32					matrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};


		gpk::err_t 		InitAudio				();
		gpk::err_t 		PrepareAudio			(::gpk::vcst_t wavname);
		gpk::err_t 		UpdateAudio				(f3_t fElapsedTime);
		gpk::err_t 		SetReverb				(int nReverb);
		gpk::err_t 		PauseAudio				(bool resume);
		gpk::err_t 		CleanupAudio			();

		gpk::err_t 		PlayWave				(::gpk::vcst_t wavname);
	};
} // namespace
#endif // GPK_XAUDIO2_H_23627


#endif // GPK_WINDOWS