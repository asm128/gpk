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
	GPK_USING_TYPEINT();

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
		u2_t				Find				(_In_z_ const sc_t * name)			const;

		bool					IsPrepared			()	noexcept;
		void					WaitOnPrepare		()	noexcept;

		bool					HasNames			()	const noexcept;
		bool					IsStreamingBank		()	const noexcept;
		const sc_t*				BankName			()	const noexcept;
		u2_t				Count				()	const noexcept;
		u2_t				BankAudioSize		()	const noexcept;
		HANDLE					GetAsyncHandle		()	const noexcept;
		u2_t				GetWaveAlignment	()	const noexcept;
		HRESULT					GetFormat			(_In_ u2_t index, _Out_writes_bytes_(maxsize) WAVEFORMATEX * pFormat, _In_ size_t maxsize)			const noexcept;
		HRESULT					GetWaveData			(_In_ u2_t index, _Outptr_ const u0_t* * pData, _Out_ u2_t & dataSize)						const noexcept;
		HRESULT					GetSeekTable		(_In_ u2_t index, _Out_ const u2_t* * pData, _Out_ u2_t & dataCount, _Out_ u2_t & tag)	const noexcept;

		struct Metadata {
			u2_t				Duration;
			u2_t				LoopStart;
			u2_t				LoopLength;
			u2_t				OffsetBytes;
			u2_t				LengthBytes;
		};
		HRESULT					GetMetadata			(_In_ u2_t index, _Out_ Metadata & metadata)	const noexcept;
	};
}

#endif // WAVEBANKREADER_H_0924289