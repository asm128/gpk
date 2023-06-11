#include "gpk_complus.h"

#ifdef GPK_WINDOWS

#include "gpk_n3.h"
#include "gpk_ptr.h"

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

#ifndef GPK_XAUDIO2_H_23611
#define GPK_XAUDIO2_H_23611

namespace gpk
{
	stacxpr uint8_t		INPUTCHANNELS	= 1;// number of source channels
	stacxpr uint8_t		OUTPUTCHANNELS	= 8;// maximum number of destination channels supported in this sample
	stacxpr uint8_t		NUM_PRESETS		= 30;

	// Constants to define our world space
	stacxpr int32_t		XMIN			= -10;
	stacxpr int32_t		XMAX			= 10;
	stacxpr int32_t		ZMIN			= -10;
	stacxpr int32_t		ZMAX			= 10;

	struct AudioDevice {
		std::wstring	Id;
		std::wstring	Description;
	};

	struct SSoundState {
		::gpk::pobj<::gpk::au8>	WaveData;
		IXAudio2SourceVoice		* SourceVoice			= 0;

		X3DAUDIO_EMITTER		Emitter					= {};
		X3DAUDIO_CONE			EmitterCone				= {};
		::gpk::n3f32			EmitterPos				= {};
		float					EmitterAzimuths			[INPUTCHANNELS]                     = {};
		float					MatrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};

		uint32_t				ChannelMask				= 0;
		uint32_t				nChannels				= 0;
		X3DAUDIO_DSP_SETTINGS	dspSettings				= {};
		bool					Playing					= false;

		HRESULT					PrepareAudio			(const ::gpk::vcc & wavname, IXAudio2*	pXAudio2, IXAudio2MasteringVoice * pMasterVoice, IXAudio2SubmixVoice * pSubmixVoice);
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

		::gpk::apod<uint8_t>	WaveData;
		IXAudio2SourceVoice		* SourceVoice			= 0;


		uint32_t				ChannelMask				= 0;
		uint32_t				nChannels				= 0;
		X3DAUDIO_DSP_SETTINGS	dspSettings				= {};

		X3DAUDIO_LISTENER		listener				= {};
		X3DAUDIO_EMITTER		emitter					= {};
		X3DAUDIO_CONE			emitterCone				= {};

		::gpk::n3f32			vListenerPos			= {};
		::gpk::n3f32			vEmitterPos				= {};
		float					fListenerAngle			= (float)(::gpk::math_pi_2 * .5);
		bool					fUseListenerCone		= true;
		bool					fUseInnerRadius			= true;
		bool					fUseRedirectToLFE		= 0;
		bool					Playing					= false;
		float					FrequencyRatio			= 1.0f;
#ifdef USING_XAUDIO2_7_DIRECTX
		uint32_t				SampleRateOriginal		= 0;
		uint32_t				SampleRateCurrent		= 0;
#endif

		FLOAT32					emitterAzimuths			[INPUTCHANNELS]                     = {};
		FLOAT32					matrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};


		::gpk::error_t			InitAudio				();
		::gpk::error_t			PrepareAudio			(const ::gpk::vcc & wavname);
		::gpk::error_t			UpdateAudio				(double fElapsedTime);
		::gpk::error_t			SetReverb				(int nReverb);
		::gpk::error_t			PauseAudio				(bool resume);
		::gpk::error_t			CleanupAudio			();

		::gpk::error_t			PlayWave				(const ::gpk::vcc & wavname);
	};
} // namespace
#endif // GPK_XAUDIO2_H_23611


#endif // GPK_WINDOWS