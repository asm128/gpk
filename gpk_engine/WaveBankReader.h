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

nmsp DirectX
{
	GPK_USING_TYPEINT();

	class WaveBankReader {
		class WaveBankReaderImpl;	// Private implementation.
		std::unique_ptr<WaveBankReaderImpl>	pImpl;
	public:
								WaveBankReader		()						nxpt(false);
								WaveBankReader		(WaveBankReader&&)		= default;
								WaveBankReader		(WaveBankReader cnst&) = delete;

		WaveBankReader&			oper=			(WaveBankReader&&)		= default;
		WaveBankReader&			oper=			(WaveBankReader cnst&)	= delete;

		HRESULT					Open				(_In_z_ cnst wchar_t * szFileName)	nxpt;
		u2_t				Find				(_In_z_ sc_c * name)			cnst;

		b8_t					IsPrepared			()	nxpt;
		void					WaitOnPrepare		()	nxpt;

		b8_t					HasNames			()	csnx;
		b8_t					IsStreamingBank		()	csnx;
		sc_c*				BankName			()	csnx;
		u2_t				Count				()	csnx;
		u2_t				BankAudioSize		()	csnx;
		HANDLE					GetAsyncHandle		()	csnx;
		u2_t				GetWaveAlignment	()	csnx;
		HRESULT					GetFormat			(_In_ u2_t index, _Out_writes_bytes_(maxsize) WAVEFORMATEX * pFormat, _In_ size_t maxsize)			csnx;
		HRESULT					GetWaveData			(_In_ u2_t index, _Outptr_ cnst u0_t* * pData, _Out_ u2_t & dataSize)						csnx;
		HRESULT					GetSeekTable		(_In_ u2_t index, _Out_ u2_c* * pData, _Out_ u2_t & dataCount, _Out_ u2_t & tag)	csnx;

		struct Metadata {
			u2_t				Duration;
			u2_t				LoopStart;
			u2_t				LoopLength;
			u2_t				OffsetBytes;
			u2_t				LengthBytes;
		};
		HRESULT					GetMetadata			(_In_ u2_t index, _Out_ Metadata & metadata)	csnx;
	};
}

#endif // WAVEBANKREADER_H_0924289