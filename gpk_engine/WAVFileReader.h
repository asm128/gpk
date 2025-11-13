// Functions for loading WAV audio files. Copyright (c) Microsoft Corporation. Licensed under the MIT License.
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
#include "gpk_array_ptr.h"
#include "gpk_array.h"

#ifdef GPK_WINDOWS

#include <objbase.h>

#include <mmreg.h>

#ifndef WAVFILEREADER_H_23627
#define WAVFILEREADER_H_23627

namespace DirectX
{
	HRESULT					LoadWAVAudioInMemory	(::gpk::vcu0_t wavData, _Outptr_ const WAVEFORMATEX* * wfx, ::gpk::vcu0_t & audioView) noexcept;
	HRESULT					LoadWAVAudioFromFile	(::gpk::vcsc_t szFileName, ::gpk::au0_t & wavData, const WAVEFORMATEX* * wfx, ::gpk::vcu0_t & audioView) noexcept;
							
	struct WAVData {		
		const WAVEFORMATEX		* WaveFormat			= 0;
		::gpk::vcu0_t				AudioView				= {};
		uint32_t				LoopStart				= 0;
		uint32_t				LoopLength				= 0;
		const uint32_t			* Seek					= 0;	// Note: XMA Seek data is Big-Endian
		uint32_t				SeekCount				= 0;
	};						
							
	HRESULT					LoadWAVAudioInMemoryEx	(::gpk::vcu0_t wavData, WAVData & result) noexcept;
	HRESULT					LoadWAVAudioFromFileEx	(::gpk::vcsc_t szFileName, ::gpk::au0_t & wavData, WAVData & result) noexcept;
}

#endif // GPK_WINDOWS

#endif // WAVFILEREADER_H_02397492830