//--------------------------------------------------------------------------------------
// File: WaveBankReader.h
//
// Functions for loading audio data from Wave Banks
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------
#include <objbase.h>
#include <mmreg.h>

#include <cstdint>
#include <memory>

#ifndef WAVEBANKREADER_H_23627
#define WAVEBANKREADER_H_23627

namespace DirectX
{
	class WaveBankReader {
		class WaveBankReaderImpl;	// Private implementation.
		std::unique_ptr<WaveBankReaderImpl>	pImpl;
	public:
								WaveBankReader		()						noexcept(false);
								WaveBankReader		(WaveBankReader&&)		= default;
								WaveBankReader		(WaveBankReader const&) = delete;

		WaveBankReader&			operator=			(WaveBankReader&&)		= default;
		WaveBankReader&			operator=			(WaveBankReader const&)	= delete;

		HRESULT					Open				(_In_z_ const wchar_t * szFileName)	noexcept;
		uint32_t				Find				(_In_z_ const char * name)			const;

		bool					IsPrepared			()	noexcept;
		void					WaitOnPrepare		()	noexcept;

		bool					HasNames			()	const noexcept;
		bool					IsStreamingBank		()	const noexcept;
		const char*				BankName			()	const noexcept;
		uint32_t				Count				()	const noexcept;
		uint32_t				BankAudioSize		()	const noexcept;
		HANDLE					GetAsyncHandle		()	const noexcept;
		uint32_t				GetWaveAlignment	()	const noexcept;
		HRESULT					GetFormat			(_In_ uint32_t index, _Out_writes_bytes_(maxsize) WAVEFORMATEX * pFormat, _In_ size_t maxsize)			const noexcept;
		HRESULT					GetWaveData			(_In_ uint32_t index, _Outptr_ const uint8_t* * pData, _Out_ uint32_t & dataSize)						const noexcept;
		HRESULT					GetSeekTable		(_In_ uint32_t index, _Out_ const uint32_t* * pData, _Out_ uint32_t & dataCount, _Out_ uint32_t & tag)	const noexcept;

		struct Metadata {
			uint32_t				Duration;
			uint32_t				LoopStart;
			uint32_t				LoopLength;
			uint32_t				OffsetBytes;
			uint32_t				LengthBytes;
		};
		HRESULT					GetMetadata			(_In_ uint32_t index, _Out_ Metadata & metadata)	const noexcept;
	};
}

#endif // WAVEBANKREADER_H_0924289