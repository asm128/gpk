//--------------------------------------------------------------------------------------
// File: WAVFileReader.cpp
//
// Functions for loading WAV audio files
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include "WAVFileReader.h"

#include "gpk_windows.h"

#include <Windows.h>
#include <cassert>
#include <memory>

using namespace DirectX;

using namespace gpk;

#ifndef MAKEFOURCC
#	define MAKEFOURCC(ch0, ch1, ch2, ch3) \
		  (static_cast<::gpk::u2_t>(static_cast<::gpk::u0_t>(ch0)) \
		| (static_cast<::gpk::u2_t>(static_cast<::gpk::u0_t>(ch1)) << 8) \
		| (static_cast<::gpk::u2_t>(static_cast<::gpk::u0_t>(ch2)) << 16) \
		| (static_cast<::gpk::u2_t>(static_cast<::gpk::u0_t>(ch3)) << 24))
#endif /* defined(MAKEFOURCC) */

namespace
{
	struct handle_closer { 
		void					operator()				(HANDLE h)	noexcept	{ 
			if (h) CloseHandle(h); 
		} 
	};

	using	ScopedHandle	= std::unique_ptr<void, handle_closer>;

	inline	HANDLE			safe_handle				(HANDLE h)	noexcept	{ 
		return (h == INVALID_HANDLE_VALUE) ? nullptr : h; 
	}

	// .WAV files
	stxp	u2_t		FOURCC_RIFF_TAG			= MAKEFOURCC('R', 'I', 'F', 'F');
	stxp	u2_t		FOURCC_FORMAT_TAG		= MAKEFOURCC('f', 'm', 't', ' ');
	stxp	u2_t		FOURCC_DATA_TAG			= MAKEFOURCC('d', 'a', 't', 'a');
	stxp	u2_t		FOURCC_WAVE_FILE_TAG	= MAKEFOURCC('W', 'A', 'V', 'E');
	stxp	u2_t		FOURCC_XWMA_FILE_TAG	= MAKEFOURCC('X', 'W', 'M', 'A');
	stxp	u2_t		FOURCC_DLS_SAMPLE		= MAKEFOURCC('w', 's', 'm', 'p');
	stxp	u2_t		FOURCC_MIDI_SAMPLE		= MAKEFOURCC('s', 'm', 'p', 'l');
	stxp	u2_t		FOURCC_XWMA_DPDS		= MAKEFOURCC('d', 'p', 'd', 's');
	stxp	u2_t		FOURCC_XMA_SEEK			= MAKEFOURCC('s', 'e', 'e', 'k');

#pragma pack(push,1)
	struct RIFFChunk {
		u2_t				Tag;
		u2_t				Size;
	};

	struct RIFFChunkHeader {
		u2_t				Tag;
		u2_t				Size;
		u2_t				Riff;
	};

	struct DLSLoop {
		stxp	u2_t		LOOP_TYPE_FORWARD		= 0x00000000;
		stxp	u2_t		LOOP_TYPE_RELEASE		= 0x00000001;

		u2_t				Size;
		u2_t				LoopType;
		u2_t				LoopStart;
		u2_t				LoopLength;
	};

	struct RIFFDLSSample
	{
		stxp	u2_t		OPTIONS_NOTRUNCATION	= 0x00000001;
		stxp	u2_t		OPTIONS_NOCOMPRESSION	= 0x00000002;

		u2_t				Size;
		u1_t				UnityNote;
		s1_t					FineTune;
		s2_t					Gain;
		u2_t				Options;
		u2_t				LoopCount;
	};

	struct MIDILoop {
		stxp	u2_t		LOOP_TYPE_FORWARD		= 0x00000000;
		stxp	u2_t		LOOP_TYPE_ALTERNATING	= 0x00000001;
		stxp	u2_t		LOOP_TYPE_BACKWARD		= 0x00000002;

		u2_t				CuePointId;
		u2_t				Type;
		u2_t				Start;
		u2_t				End;
		u2_t				Fraction;
		u2_t				PlayCount;
	};

	struct RIFFMIDISample {
		u2_t				ManufacturerId;
		u2_t				ProductId;
		u2_t				SamplePeriod;
		u2_t				UnityNode;
		u2_t				PitchFraction;
		u2_t				SMPTEFormat;
		u2_t				SMPTEOffset;
		u2_t				LoopCount;
		u2_t				SamplerData;
	};
#pragma pack(pop)

	static_assert( 8 == sizeof(RIFFChunk)		, "structure size mismatch");
	static_assert(12 == sizeof(RIFFChunkHeader)	, "structure size mismatch");
	static_assert(16 == sizeof(DLSLoop)			, "structure size mismatch");
	static_assert(20 == sizeof(RIFFDLSSample)	, "structure size mismatch");
	static_assert(24 == sizeof(MIDILoop)		, "structure size mismatch");
	static_assert(36 == sizeof(RIFFMIDISample)	, "structure size mismatch");

	//---------------------------------------------------------------------------------
	const RIFFChunk* FindChunk(::gpk::vcu0_t data, _In_ u2_t tag) noexcept {
		const u0_t* ptr = data.begin();
		const u0_t* end = data.end();
		while (end > (ptr + sizeof(RIFFChunk))) {
			auto header = reinterpret_cast<const RIFFChunk*>(ptr);
			if (header->Tag == tag)
				return header;

			auto const offset = header->Size + sizeof(RIFFChunk);
			ptr += offset;
		}

		return nullptr;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindFormatAndData(::gpk::vcu0_t wavData, _Outptr_ const WAVEFORMATEX* * pwfx, ::gpk::vcu0_t & data, _Out_ bool & dpds, _Out_ bool & seek) noexcept {
		dpds = seek = false;
		if_true_ve(E_FAIL, wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(u2_t) + sizeof(WAVEFORMAT)));

		const u0_t* wavEnd = wavData.begin() + wavData.size();

		// Locate RIFF 'WAVE'
		auto riffChunk = FindChunk(wavData, FOURCC_RIFF_TAG);
		if_true_ve(E_FAIL, (!riffChunk || riffChunk->Size < 4));

		auto riffHeader = reinterpret_cast<const RIFFChunkHeader*>(riffChunk);
		if_true_ve(E_FAIL, (riffHeader->Riff != FOURCC_WAVE_FILE_TAG && riffHeader->Riff != FOURCC_XWMA_FILE_TAG));

		// Locate 'fmt '
		auto ptr = reinterpret_cast<const u0_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		if_true_ve(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), ((ptr + sizeof(RIFFChunk)) > wavEnd));

		auto fmtChunk = FindChunk({ptr, riffHeader->Size}, FOURCC_FORMAT_TAG);
		if_true_ve(E_FAIL, (!fmtChunk || fmtChunk->Size < sizeof(PCMWAVEFORMAT)));

		ptr = reinterpret_cast<const u0_t*>(fmtChunk) + sizeof(RIFFChunk);
		if_true_ve(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), (ptr + fmtChunk->Size > wavEnd));

		auto wf = reinterpret_cast<const WAVEFORMAT*>(ptr);

		// Validate WAVEFORMAT (focused on chunk size and format tag, not other data that XAUDIO2 will validate)
		switch (wf->wFormatTag)
		{
			case WAVE_FORMAT_PCM:
			case WAVE_FORMAT_IEEE_FLOAT:
				// Can be a PCMWAVEFORMAT (16 bytes) or WAVEFORMATEX (18 bytes)
				// We validiated chunk as at least sizeof(PCMWAVEFORMAT) above
				break;

			default:
			{
				if (fmtChunk->Size < sizeof(WAVEFORMATEX))
					return E_FAIL;

				auto wfx = reinterpret_cast<const WAVEFORMATEX*>(ptr);

				if (fmtChunk->Size < (sizeof(WAVEFORMATEX) + wfx->cbSize))
					return E_FAIL;

				switch (wfx->wFormatTag) {
					case WAVE_FORMAT_WMAUDIO2:
					case WAVE_FORMAT_WMAUDIO3:
						dpds = true;
						break;

					case  0x166 /*WAVE_FORMAT_XMA2*/: // XMA2 is supported by Xbox One
						if_true_ve(E_FAIL, ((fmtChunk->Size < 52 /*sizeof(XMA2WAVEFORMATEX)*/) || (wfx->cbSize < 34 /*( sizeof(XMA2WAVEFORMATEX) - sizeof(WAVEFORMATEX) )*/)));
						seek = true;
						break;

					case WAVE_FORMAT_ADPCM:
						if_true_ve(E_FAIL, ((fmtChunk->Size < (sizeof(WAVEFORMATEX) + 32)) || (wfx->cbSize < 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/)));
						break;

					case WAVE_FORMAT_EXTENSIBLE:
						if_true_ve(E_FAIL, ((fmtChunk->Size < sizeof(WAVEFORMATEXTENSIBLE)) || (wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)))))
						else {
							static const GUID s_wfexBase = { 0x00000000, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

							auto wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(ptr);

							if_true_ve(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED), (memcmp(reinterpret_cast<const BYTE*>(&wfex->SubFormat) + sizeof(DWORD),
								reinterpret_cast<const BYTE*>(&s_wfexBase) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD)) != 0));

							switch (wfex->SubFormat.Data1) {
								case WAVE_FORMAT_PCM:
								case WAVE_FORMAT_IEEE_FLOAT:
									break;

								// MS-ADPCM and XMA2 are not supported as WAVEFORMATEXTENSIBLE

								case WAVE_FORMAT_WMAUDIO2:
								case WAVE_FORMAT_WMAUDIO3:
									dpds = true;
									break;

								default:
									return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
							}

						}
						break;

					default:
						return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
				}
			}
		}

		// Locate 'data'
		ptr = reinterpret_cast<const u0_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		if_true_ve(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), ((ptr + sizeof(RIFFChunk)) > wavEnd));
		auto dataChunk = FindChunk({ptr, riffChunk->Size}, FOURCC_DATA_TAG);
		if_true_ve(HRESULT_FROM_WIN32(ERROR_INVALID_DATA), (!dataChunk || !dataChunk->Size));
		ptr = reinterpret_cast<const u0_t*>(dataChunk) + sizeof(RIFFChunk);
		if_true_ve(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), (ptr + dataChunk->Size > wavEnd));
		*pwfx = reinterpret_cast<const WAVEFORMATEX*>(wf);
		data = {ptr, dataChunk->Size};
		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindLoopInfo(::gpk::vcu0_t wavData, _Out_ u2_t* pLoopStart, _Out_ u2_t* pLoopLength) noexcept {
		if (wavData.size() < (sizeof(RIFFChunk) + sizeof(u2_t)))
			return E_FAIL;

		*pLoopStart = 0;
		*pLoopLength = 0;

		const u0_t			* wavEnd		= wavData.end();

		// Locate RIFF 'WAVE'
		const RIFFChunk			* riffChunk		= FindChunk(wavData, FOURCC_RIFF_TAG);
		if (!riffChunk || riffChunk->Size < 4)
			return E_FAIL;

		const RIFFChunkHeader	* riffHeader	= (const RIFFChunkHeader*)riffChunk;
		if (riffHeader->Riff == FOURCC_XWMA_FILE_TAG)
			return S_OK; // xWMA files do not contain loop information

		if_true_ve(-1, riffHeader->Riff != FOURCC_WAVE_FILE_TAG);

		// Locate 'wsmp' (DLS Chunk)
		const u0_t			* ptr			= ((const u0_t*)riffHeader) + sizeof(RIFFChunkHeader);
		if ((ptr + sizeof(RIFFChunk)) > wavEnd)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		auto dlsChunk = FindChunk({ptr, riffChunk->Size}, FOURCC_DLS_SAMPLE);
		if (dlsChunk) {
			ptr = ((const u0_t*)dlsChunk) + sizeof(RIFFChunk);
			if (ptr + dlsChunk->Size > wavEnd)
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

			if (dlsChunk->Size >= sizeof(RIFFDLSSample)) {
				const RIFFDLSSample		* dlsSample		= (const RIFFDLSSample*)ptr;

				if (dlsChunk->Size >= (dlsSample->Size + dlsSample->LoopCount * sizeof(DLSLoop))) {
					const DLSLoop			* loops			= (const DLSLoop*)(ptr + dlsSample->Size);
					for (u2_t j = 0; j < dlsSample->LoopCount; ++j) {
						if ((loops[j].LoopType == DLSLoop::LOOP_TYPE_FORWARD || loops[j].LoopType == DLSLoop::LOOP_TYPE_RELEASE)) {
							// Return 'forward' loop
							*pLoopStart = loops[j].LoopStart;
							*pLoopLength = loops[j].LoopLength;
							return S_OK;
						}
					}
				}
			}
		}

		// Locate 'smpl' (Sample Chunk)
		const RIFFChunk			* midiChunk		= FindChunk({ptr, riffChunk->Size}, FOURCC_MIDI_SAMPLE);
		if (midiChunk) {
			ptr					= ((const u0_t*)midiChunk) + sizeof(RIFFChunk);
			if (ptr + midiChunk->Size > wavEnd)
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

			if (midiChunk->Size >= sizeof(RIFFMIDISample)) {
				const RIFFMIDISample	* midiSample		= (const RIFFMIDISample*)ptr;

				if (midiChunk->Size >= (sizeof(RIFFMIDISample) + midiSample->LoopCount * sizeof(MIDILoop))) {
					auto loops = (const MIDILoop*)(ptr + sizeof(RIFFMIDISample));
					for (u2_t j = 0; j < midiSample->LoopCount; ++j) {
						if (loops[j].Type == MIDILoop::LOOP_TYPE_FORWARD) {
							// Return 'forward' loop
							*pLoopStart = loops[j].Start;
							*pLoopLength = loops[j].End - loops[j].Start + 1;
							return S_OK;
						}
					}
				}
			}
		}

		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindTable(::gpk::vcu0_t wavData, _In_ u2_t tag, _Outptr_result_maybenull_ const u2_t* * pData, _Out_ u2_t * dataCount) noexcept {
		if_true_ve(E_FAIL, wavData.size() < (sizeof(RIFFChunk) + sizeof(u2_t)));

		*pData				= nullptr;
		*dataCount			= 0;
							
		const u0_t			* wavEnd		= wavData.end();

		// Locate RIFF 'WAVE'
		auto					riffChunk		= FindChunk(wavData, FOURCC_RIFF_TAG);
		if (!riffChunk || riffChunk->Size < 4)
			return E_FAIL;

		auto					riffHeader		= reinterpret_cast<const RIFFChunkHeader*>(riffChunk);
		if (riffHeader->Riff != FOURCC_WAVE_FILE_TAG && riffHeader->Riff != FOURCC_XWMA_FILE_TAG)
			return E_FAIL;

		// Locate tag
		auto					ptr				= (const u0_t*)(riffHeader) + sizeof(RIFFChunkHeader);
		if ((ptr + sizeof(RIFFChunk)) > wavEnd)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		auto					tableChunk		= FindChunk({ptr, riffChunk->Size}, tag);
		if (tableChunk) {
			ptr					= reinterpret_cast<const u0_t*>(tableChunk) + sizeof(RIFFChunk);
			if (ptr + tableChunk->Size > wavEnd)
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

			if ((tableChunk->Size % sizeof(u2_t)) != 0)
				return E_FAIL;

			*pData				= reinterpret_cast<const u2_t*>(ptr);
			*dataCount			= tableChunk->Size / 4;
		}

		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT LoadAudioFromFile(vcst_t szFileName, _Inout_ ::gpk::au0_t & wavData) noexcept { 
		DWORD								bytesRead		   = 0;
		::gpk::apod<wchar_t>				wszFileName		 = {};
		::gpk::mbstowcs(wszFileName, szFileName);
		// open the file
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		ScopedHandle						hFile				(safe_handle(CreateFile2(wszFileName.begin(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
		ScopedHandle						hFile				(safe_handle(CreateFileW(wszFileName.begin(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

		if_true_vef(HRESULT_FROM_WIN32(GetLastError()), !hFile, "%.*s", GPK_VIEW_ARGS(szFileName));

		// Get the file size
		FILE_STANDARD_INFO					fileInfo			= {};
		if_true_vef(HRESULT_FROM_WIN32(GetLastError()), !GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)), "File not found: '%.*s'", GPK_VIEW_ARGS(szFileName));
		if_true_vef(E_FAIL, fileInfo.EndOfFile.HighPart > 0, "File is too big (%llu), so reject read", fileInfo.EndOfFile.QuadPart);
		if_true_vef(E_FAIL, fileInfo.EndOfFile.LowPart < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT)), "Need at least %u bytes to have a valid minimal WAV file. Actual size: %u", sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT), fileInfo.EndOfFile.LowPart);

		wavData.resize(fileInfo.EndOfFile.LowPart);
		if (FALSE == ReadFile(hFile.get(), wavData.begin(), fileInfo.EndOfFile.LowPart, &bytesRead, nullptr)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}
		return (bytesRead < fileInfo.EndOfFile.LowPart) ? E_FAIL : bytesRead;
	}
}

//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::LoadWAVAudioInMemory(::gpk::vcu0_t wavData, _Outptr_ const WAVEFORMATEX* * wfx, ::gpk::vcu0_t & audioBytes) noexcept {
	if_true_vef(E_FAIL, (wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT))), "Need at least enough data to have a valid minimal WAV file. Size: %i", wavData.size());

	bool dpds = true, seek = true;
	gpk_hrcall(WaveFindFormatAndData(wavData, wfx, audioBytes, dpds, seek));

	return (dpds || seek) ? E_FAIL : S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT DirectX::LoadWAVAudioFromFile(::gpk::vcst_t szFileName, ::gpk::au0_t & wavData, const WAVEFORMATEX* * wfx, ::gpk::vcu0_t & audioBytes) noexcept {
	gpk_hrcall(LoadAudioFromFile(szFileName, wavData));

	bool dpds = true, seek = true;
	gpk_hrcall(WaveFindFormatAndData(wavData, wfx, audioBytes, dpds, seek));

	return (dpds || seek) ? E_FAIL : S_OK;
}

//-------------------------------------------------------------------------------------
HRESULT DirectX::LoadWAVAudioInMemoryEx(::gpk::vcu0_t wavData, DirectX::WAVData & result) noexcept {
	// Need at least enough data to have a valid minimal WAV file
	if (wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT))) {
		return E_FAIL;
	}

	bool dpds = false, seek = false;
	gpk_hrcall(WaveFindFormatAndData(wavData, &result.WaveFormat, result.AudioView, dpds, seek));
	gpk_hrcall(WaveFindLoopInfo(wavData, &result.LoopStart, &result.LoopLength));

	if (dpds) {
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XWMA_DPDS, &result.Seek, &result.SeekCount));
	}
	else if (seek)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XMA_SEEK, &result.Seek, &result.SeekCount));

	return S_OK;
}

//-------------------------------------------------------------------------------------
HRESULT DirectX::LoadWAVAudioFromFileEx(::gpk::vcst_t szFileName, ::gpk::au0_t & wavData, DirectX::WAVData & result) noexcept {
	DWORD bytesRead = LoadAudioFromFile(szFileName, wavData);
	if (FAILED(bytesRead))
		return bytesRead;

	bool dpds = false, seek = false;
	gpk_hrcall(WaveFindFormatAndData(wavData, &result.WaveFormat, result.AudioView, dpds, seek));
	gpk_hrcall(WaveFindLoopInfo(wavData, &result.LoopStart, &result.LoopLength));

	if (dpds) {
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XWMA_DPDS, &result.Seek, &result.SeekCount));
	}
	else if (seek)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XMA_SEEK, &result.Seek, &result.SeekCount));

	return S_OK;
}

