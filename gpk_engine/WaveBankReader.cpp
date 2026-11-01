//--------------------------------------------------------------------------------------
// File: WaveBankReader.cpp
//
// Functions for loading audio data from Wave Banks
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#include "gpk_complus.h"

#ifdef GPK_WINDOWS

#include <cassert>

#include "WaveBankReader.h"

#include <map>
#include <string>
#include <tuple>

GPK_USING_TYPEINT();

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
				(static_cast<u2_t>(static_cast<u0_t>(ch0)) \
				| (static_cast<u2_t>(static_cast<u0_t>(ch1)) << 8) \
				| (static_cast<u2_t>(static_cast<u0_t>(ch2)) << 16) \
				| (static_cast<u2_t>(static_cast<u0_t>(ch3)) << 24))
#endif /* defined(MAKEFOURCC) */


namespace
{
	struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

	using ScopedHandle = std::unique_ptr<void, handle_closer>;

	inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }

#pragma pack(push, 1)

	constexpr size_t DVD_SECTOR_SIZE = 2048;
	constexpr size_t DVD_BLOCK_SIZE = DVD_SECTOR_SIZE * 16;

	constexpr size_t ALIGNMENT_MIN = 4;
	constexpr size_t ALIGNMENT_DVD = DVD_SECTOR_SIZE;

	constexpr size_t MAX_DATA_SEGMENT_SIZE = 0xFFFFFFFF;
	constexpr size_t MAX_COMPACT_DATA_SEGMENT_SIZE = 0x001FFFFF;

	struct REGION
	{
		u2_t    dwOffset;   // Region offset, in bytes.
		u2_t    dwLength;   // Region length, in bytes.
	};

	struct SAMPLEREGION
	{
		u2_t    dwStartSample;  // Start sample for the region.
		u2_t    dwTotalSamples; // Region length in samples.
	};

	struct HEADER
	{
		static constexpr u2_t SIGNATURE = MAKEFOURCC('W', 'B', 'N', 'D');
		static constexpr u2_t BE_SIGNATURE = MAKEFOURCC('D', 'N', 'B', 'W');
		static constexpr u2_t VERSION = 44;

		enum SEGIDX
		{
			SEGIDX_BANKDATA = 0,       // Bank data
			SEGIDX_ENTRYMETADATA,      // Entry meta-data
			SEGIDX_SEEKTABLES,         // Storage for seek tables for the encoded waves.
			SEGIDX_ENTRYNAMES,         // Entry friendly names
			SEGIDX_ENTRYWAVEDATA,      // Entry wave data
			SEGIDX_COUNT
		};

		u2_t    dwSignature;            // File signature
		u2_t    dwVersion;              // Version of the tool that created the file
		u2_t    dwHeaderVersion;        // Version of the file format
		REGION      Segments[SEGIDX_COUNT]; // Segment lookup table
	};

#pragma warning( disable : 4201 4203 )

	union MINIWAVEFORMAT {
		static constexpr u2_t TAG_PCM = 0x0;
		static constexpr u2_t TAG_XMA = 0x1;
		static constexpr u2_t TAG_ADPCM = 0x2;
		static constexpr u2_t TAG_WMA = 0x3;

		static constexpr u2_t BITDEPTH_8 = 0x0; // PCM only
		static constexpr u2_t BITDEPTH_16 = 0x1; // PCM only

		static constexpr size_t ADPCM_BLOCKALIGN_CONVERSION_OFFSET = 22;

		struct
		{
			u2_t       wFormatTag : 2;        // Format tag
			u2_t       nChannels : 3;        // Channel count (1 - 6)
			u2_t       nSamplesPerSec : 18;       // Sampling rate
			u2_t       wBlockAlign : 8;        // Block alignment.  For WMA, lower 6 bits block alignment index, upper 2 bits bytes-per-second index.
			u2_t       wBitsPerSample : 1;        // Bits per sample (8 vstr_t. 16, PCM only); WMAudio2/WMAudio3 (for WMA)
		};

		u2_t           dwValue;

		WORD	BitsPerSample	()	const noexcept	{
			if (wFormatTag == TAG_XMA)
				return 16; // XMA_OUTPUT_SAMPLE_BITS == 16
			if (wFormatTag == TAG_WMA)
				return 16;
			if (wFormatTag == TAG_ADPCM)
				return 4; // MSADPCM_BITS_PER_SAMPLE == 4
			
			return (wBitsPerSample == BITDEPTH_16) ? 16u : 8u;	// wFormatTag must be TAG_PCM (2 bits can only represent 4 different values)
		}

		DWORD	BlockAlign		()	const noexcept	{
			switch (wFormatTag) {
			case TAG_PCM	: return wBlockAlign;
			case TAG_XMA	: return (nChannels * 16 / 8); // XMA_OUTPUT_SAMPLE_BITS = 16
			case TAG_ADPCM	: return (wBlockAlign + ADPCM_BLOCKALIGN_CONVERSION_OFFSET) * nChannels;
			case TAG_WMA	: {
				static const u2_t aWMABlockAlign[17] =
					{ 929
					, 1487
					, 1280
					, 2230
					, 8917
					, 8192
					, 4459
					, 5945
					, 2304
					, 1536
					, 1485
					, 1008
					, 2731
					, 4096
					, 6827
					, 5462
					, 1280
					};
				const u2_t dwBlockAlignIndex = wBlockAlign & 0x1F;
				if (dwBlockAlignIndex < 17)
					return aWMABlockAlign[dwBlockAlignIndex];
			}
			break;
			}
			return 0;
		}

		DWORD	AvgBytesPerSec() const noexcept {
			switch (wFormatTag) {
				case TAG_PCM	: return nSamplesPerSec * wBlockAlign;
				case TAG_XMA	: return nSamplesPerSec * BlockAlign();
				case TAG_ADPCM	: {
					const u2_t blockAlign			= BlockAlign();
					const u2_t samplesPerAdpcmBlock	= AdpcmSamplesPerBlock();
					return blockAlign * nSamplesPerSec / samplesPerAdpcmBlock;
				}
				case TAG_WMA	: {
					static const u2_t aWMAAvgBytesPerSec[7] =
					{	12000
					,	24000
					,	4000
					,	6000
					,	8000
					,	20000
					,	2500
					};
					// bitrate = entry * 8

					const u2_t dwBytesPerSecIndex = wBlockAlign >> 5;
					if (dwBytesPerSecIndex < 7)
						return aWMAAvgBytesPerSec[dwBytesPerSecIndex];
				}
				break;
			}

			return 0;
		}

		DWORD AdpcmSamplesPerBlock() const noexcept {
			const u2_t nBlockAlign = (wBlockAlign + ADPCM_BLOCKALIGN_CONVERSION_OFFSET) * nChannels;
			return nBlockAlign * 2 / u2_t(nChannels) - 12;
		}

		void AdpcmFillCoefficientTable(ADPCMWAVEFORMAT *fmt) const noexcept { 
			fmt->wNumCoef = 7 /* MSADPCM_NUM_COEFFICIENTS */;// These are fixed since we are always using MS ADPCM
			static ADPCMCOEFSET aCoef[7] = { { 256, 0}, {512, -256}, {0,0}, {192,64}, {240,0}, {460, -208}, {392,-232} };
			memcpy(&fmt->aCoef, aCoef, sizeof(aCoef));
		}
	};

	struct BANKDATA {
		static constexpr size_t BANKNAME_LENGTH = 64;

		static constexpr u2_t TYPE_BUFFER			= 0x00000000;
		static constexpr u2_t TYPE_STREAMING		= 0x00000001;
		static constexpr u2_t TYPE_MASK				= 0x00000001;

		static constexpr u2_t FLAGS_ENTRYNAMES		= 0x00010000;
		static constexpr u2_t FLAGS_COMPACT			= 0x00020000;
		static constexpr u2_t FLAGS_SYNC_DISABLED	= 0x00040000;
		static constexpr u2_t FLAGS_SEEKTABLES		= 0x00080000;
		static constexpr u2_t FLAGS_MASK			= 0x000F0000;

		u2_t		dwFlags							= {}; // Bank flags
		u2_t		dwEntryCount					= {}; // Number of entries in the bank
		sc_t			szBankName[BANKNAME_LENGTH]		= {}; // Bank friendly name
		u2_t		dwEntryMetaDataElementSize		= {}; // Size of each entry meta-data element, in bytes
		u2_t		dwEntryNameElementSize			= {}; // Size of each entry name element, in bytes
		u2_t		dwAlignment						= {}; // Entry alignment, in bytes
		MINIWAVEFORMAT	CompactFormat					= {}; // Format data for compact bank
		FILETIME		BuildTime						= {}; // Build timestamp
	};

	struct ENTRY {
		static constexpr u2_t FLAGS_READAHEAD		= 0x00000001;	// Enable stream read-ahead
		static constexpr u2_t FLAGS_LOOPCACHE		= 0x00000002;	// One or more looping sounds use this wave
		static constexpr u2_t FLAGS_REMOVELOOPTAIL	= 0x00000004;	// Remove data after the end of the loop region
		static constexpr u2_t FLAGS_IGNORELOOP		= 0x00000008;	// Used internally when the loop region can't be used
		static constexpr u2_t FLAGS_MASK			= 0x0000000F;

		union {
			struct {
				u2_t		Flags		: 4;	// Entry flags
				u2_t		Duration	: 28;	// Duration of the wave, in units of one sample. For instance, a ten second long wave sampled at 48KHz would have a duration of 480,000. // This value is not affected by the number of channels, the number of bits per sample, or the compression format of the wave.
			};
			u2_t		dwFlagsAndDuration;
		};

		MINIWAVEFORMAT	Format;         // Entry format.
		REGION			PlayRegion;     // Region within the wave data segment that contains this entry.
		SAMPLEREGION	LoopRegion;     // Region within the wave data (in samples) that should loop.
	};

	struct ENTRYCOMPACT {
		u2_t		dwOffset : 21;       // Data offset, in multiplies of the bank alignment
		u2_t		dwLengthDeviation : 11;       // Data length deviation, in bytes

		void ComputeLocations(DWORD& offset, DWORD& length, u2_t index, const HEADER& header, const BANKDATA& data, const ENTRYCOMPACT* entries) const noexcept {
			offset = dwOffset * data.dwAlignment;

			if (index < (data.dwEntryCount - 1))
				length = (entries[index + 1].dwOffset * data.dwAlignment) - offset - dwLengthDeviation;
			else
				length = header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength - offset - dwLengthDeviation;
		}

		static u2_t GetDuration(DWORD length, const BANKDATA& data, const u2_t* seekTable) noexcept {
			switch (data.CompactFormat.wFormatTag) {
				case MINIWAVEFORMAT::TAG_ADPCM: {
					u2_t duration = (length / data.CompactFormat.BlockAlign()) * data.CompactFormat.AdpcmSamplesPerBlock();
					const u2_t partial = length % data.CompactFormat.BlockAlign();
					if (partial) {
						if (partial >= (7u * data.CompactFormat.nChannels))
							duration += (partial * 2 / data.CompactFormat.nChannels - 12);
					}
					return duration;
				}
				case MINIWAVEFORMAT::TAG_WMA:
					if (0 == seekTable) {
						const u2_t seekCount = *seekTable;
						if (seekCount > 0)
							return seekTable[seekCount] / u2_t(2 * data.CompactFormat.nChannels);
					}
					return 0;
				case MINIWAVEFORMAT::TAG_XMA:
					if (seekTable) {
						const u2_t seekCount = *seekTable;
						if (seekCount > 0)
							return seekTable[seekCount];
					}
					return 0;
				default:
					return u2_t((u3_t(length) * 8) / (u3_t(data.CompactFormat.BitsPerSample()) * u3_t(data.CompactFormat.nChannels)));
			}
		}
	};

#pragma pack(pop)

	inline const u2_t* FindSeekTable(u2_t index, const u0_t* seekTable, const HEADER& header, const BANKDATA& data) noexcept {
		if (!seekTable || index >= data.dwEntryCount)
			return nullptr;

		const u2_t seekSize = header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength;

		if ((index * sizeof(u2_t)) > seekSize)
			return nullptr;

		auto table = reinterpret_cast<const u2_t*>(seekTable);
		u2_t offset = table[index];
		if (offset == u2_t(-1))
			return nullptr;

		offset += sizeof(u2_t) * data.dwEntryCount;

		if (offset > seekSize)
			return nullptr;

		return reinterpret_cast<const u2_t*>(seekTable + offset);
	}
}

static_assert(sizeof(REGION) == 8, "Mismatch with xact3wb.h");
static_assert(sizeof(SAMPLEREGION) == 8, "Mismatch with xact3wb.h");
static_assert(sizeof(HEADER) == 52, "Mismatch with xact3wb.h");
static_assert(sizeof(ENTRY) == 24, "Mismatch with xact3wb.h");
static_assert(sizeof(MINIWAVEFORMAT) == 4, "Mismatch with xact3wb.h");
static_assert(sizeof(ENTRY) == 24, "Mismatch with xact3wb.h");
static_assert(sizeof(ENTRYCOMPACT) == 4, "Mismatch with xact3wb.h");
static_assert(sizeof(BANKDATA) == 96, "Mismatch with xact3wb.h");

using namespace DirectX;

//--------------------------------------------------------------------------------------
class WaveBankReader::WaveBankReaderImpl
{
	std::unique_ptr<u0_t[]>		m_entries;
	std::unique_ptr<u0_t[]>		m_seekData;
	std::unique_ptr<u0_t[]>		m_waveData;
public:
	HANDLE							m_async;
	ScopedHandle					m_event;
	OVERLAPPED						m_request;
	bool							m_prepared;

	HEADER							m_header;
	BANKDATA						m_data;
	std::map<std::string, u2_t>	m_names;	

									~WaveBankReaderImpl	()				{ Close(); }
									WaveBankReaderImpl	(WaveBankReaderImpl&&)		= default;
									WaveBankReaderImpl	(WaveBankReaderImpl const&)	= delete;

									WaveBankReaderImpl	()	noexcept
		: m_async(INVALID_HANDLE_VALUE)
		, m_request{}
		, m_prepared(false)
		, m_header{}
		, m_data{}
	{}

	WaveBankReaderImpl&				operator=			(WaveBankReaderImpl&&)		= default;
	WaveBankReaderImpl&				operator=			(WaveBankReaderImpl const&)	= delete;

	HRESULT							Open				(_In_z_ const wchar_t* szFileName)	noexcept(false);
	void							Close				()									noexcept;

	HRESULT							GetFormat			(_In_ u2_t index, _Out_writes_bytes_(maxsize) WAVEFORMATEX* pFormat, _In_ size_t maxsize)		const noexcept;
	HRESULT							GetWaveData			(_In_ u2_t index, _Outptr_ const u0_t** pData, _Out_ u2_t& dataSize)						const noexcept;
	HRESULT							GetSeekTable		(_In_ u2_t index, _Out_ const u2_t** pData, _Out_ u2_t& dataCount, _Out_ u2_t& tag)	const noexcept;
	HRESULT							GetMetadata			(_In_ u2_t index, _Out_ Metadata& metadata)														const noexcept;

	bool							UpdatePrepared		() noexcept;
	void							Clear				() noexcept {
		memset(&m_header, 0, sizeof(HEADER));
		memset(&m_data, 0, sizeof(BANKDATA));
		m_names.clear();
		m_entries.reset();
		m_seekData.reset();
		m_waveData.reset();
	}


};


_Use_decl_annotations_
HRESULT WaveBankReader::WaveBankReaderImpl::Open(const wchar_t* szFileName) noexcept(false) {
	Close();
	Clear();

	m_prepared = false;

	m_event.reset(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
	if (!m_event)
		return HRESULT_FROM_WIN32(GetLastError());

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	CREATEFILE2_EXTENDED_PARAMETERS params = { sizeof(CREATEFILE2_EXTENDED_PARAMETERS), 0, 0, 0, {}, nullptr };
	params.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	params.dwFileFlags = FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN;
	ScopedHandle hFile(safe_handle(CreateFile2(
		szFileName,
		GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
		&params)));
#else
	ScopedHandle hFile(safe_handle(CreateFileW(
		szFileName,
		GENERIC_READ, FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN,
		nullptr)));
#endif

	if (!hFile)
		return HRESULT_FROM_WIN32(GetLastError());

	// Read and verify header
	OVERLAPPED request = {};
	request.hEvent = m_event.get();

	bool wait = false;
	if (!ReadFile(hFile.get(), &m_header, sizeof(m_header), nullptr, &request)) {
		const DWORD error = GetLastError();
		if (error != ERROR_IO_PENDING)
			return HRESULT_FROM_WIN32(error);
		wait = true;
	}

	DWORD bytes;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	std::ignore = wait;

	BOOL result = GetOverlappedResultEx(hFile.get(), &request, &bytes, INFINITE, FALSE);
#else
	if (wait)
		std::ignore = WaitForSingleObject(m_event.get(), INFINITE);

	BOOL result = GetOverlappedResult(hFile.get(), &request, &bytes, FALSE);
#endif

	if (!result || (bytes != sizeof(m_header))) 
		return HRESULT_FROM_WIN32(GetLastError());

	if_true_ve(E_FAIL, m_header.dwSignature != HEADER::SIGNATURE && m_header.dwSignature != HEADER::BE_SIGNATURE);

	const bool be = (m_header.dwSignature == HEADER::BE_SIGNATURE);
	if (be) {
		return E_FAIL;
	}

	if_true_ve(E_FAIL, m_header.dwHeaderVersion != HEADER::VERSION);

	// Load bank data
	memset(&request, 0, sizeof(request));
	request.Offset = m_header.Segments[HEADER::SEGIDX_BANKDATA].dwOffset;
	request.hEvent = m_event.get();

	wait = false;
	if (!ReadFile(hFile.get(), &m_data, sizeof(m_data), nullptr, &request)) {
		const DWORD error = GetLastError();
		if (error != ERROR_IO_PENDING)
			return HRESULT_FROM_WIN32(error);
		wait = true;
	}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	result = GetOverlappedResultEx(hFile.get(), &request, &bytes, INFINITE, FALSE);
#else
	if (wait) {
		std::ignore = WaitForSingleObject(m_event.get(), INFINITE);
	}

	result = GetOverlappedResult(hFile.get(), &request, &bytes, FALSE);
#endif

	if (!result || (bytes != sizeof(m_data)))
		return HRESULT_FROM_WIN32(GetLastError());

	if (!m_data.dwEntryCount)
		return HRESULT_FROM_WIN32(ERROR_NO_DATA);

	if (m_data.dwFlags & BANKDATA::TYPE_STREAMING) {
		if (m_data.dwAlignment < ALIGNMENT_DVD)
			return E_FAIL;
		if (m_data.dwAlignment % DVD_SECTOR_SIZE)
			return E_FAIL;
	}
	else if (m_data.dwAlignment < ALIGNMENT_MIN)
		return E_FAIL;

	if (m_data.dwFlags & BANKDATA::FLAGS_COMPACT) {
		if_true_ve(E_FAIL, m_data.dwEntryMetaDataElementSize != sizeof(ENTRYCOMPACT));
		if_true_ve(E_FAIL, m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength > (MAX_COMPACT_DATA_SEGMENT_SIZE * m_data.dwAlignment)); // Data segment is too large to be valid compact wavebank
	}
	else {
		if_true_ve(E_FAIL, m_data.dwEntryMetaDataElementSize != sizeof(ENTRY));
	}

	const DWORD metadataBytes = m_header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwLength;
	if_true_ve(E_FAIL, metadataBytes != (m_data.dwEntryCount * m_data.dwEntryMetaDataElementSize));

	// Load names
	const DWORD namesBytes = m_header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwLength;
	if (namesBytes > 0) {
		if (namesBytes >= (m_data.dwEntryNameElementSize * m_data.dwEntryCount))
		{
			std::unique_ptr<sc_t[]> temp(new (std::nothrow) sc_t[namesBytes]);
			if (!temp)
				return E_OUTOFMEMORY;

			memset(&request, 0, sizeof(request));
			request.Offset = m_header.Segments[HEADER::SEGIDX_ENTRYNAMES].dwOffset;
			request.hEvent = m_event.get();

			wait = false;
			if (!ReadFile(hFile.get(), temp.get(), namesBytes, nullptr, &request))
			{
				const DWORD error = GetLastError();
				if (error != ERROR_IO_PENDING)
					return HRESULT_FROM_WIN32(error);
				wait = true;
			}

		#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
			result = GetOverlappedResultEx(hFile.get(), &request, &bytes, INFINITE, FALSE);
		#else
			if (wait)
			{
				std::ignore = WaitForSingleObject(m_event.get(), INFINITE);
			}

			result = GetOverlappedResult(hFile.get(), &request, &bytes, FALSE);
		#endif

			if_true_ve(HRESULT_FROM_WIN32(GetLastError()), !result || (namesBytes != bytes));
				
			for (u2_t j = 0; j < m_data.dwEntryCount; ++j) {
				const DWORD n = m_data.dwEntryNameElementSize * j;

				sc_t name[64] = {};
				strncpy_s(name, &temp[n], sizeof(name));

				m_names[name] = j;
			}
		}
	}

	// Load entries
	if (m_data.dwFlags & BANKDATA::FLAGS_COMPACT)
		m_entries.reset(reinterpret_cast<u0_t*>(new (std::nothrow) ENTRYCOMPACT[m_data.dwEntryCount]));
	else
		m_entries.reset(reinterpret_cast<u0_t*>(new (std::nothrow) ENTRY[m_data.dwEntryCount]));

	if (!m_entries)
		return E_OUTOFMEMORY;

	memset(&request, 0, sizeof(request));
	request.Offset = m_header.Segments[HEADER::SEGIDX_ENTRYMETADATA].dwOffset;
	request.hEvent = m_event.get();

	wait = false;
	if (!ReadFile(hFile.get(), m_entries.get(), metadataBytes, nullptr, &request))
	{
		const DWORD error = GetLastError();
		if (error != ERROR_IO_PENDING)
			return HRESULT_FROM_WIN32(error);
		wait = true;
	}

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	result = GetOverlappedResultEx(hFile.get(), &request, &bytes, INFINITE, FALSE);
#else
	if (wait)
	{
		std::ignore = WaitForSingleObject(m_event.get(), INFINITE);
	}

	result = GetOverlappedResult(hFile.get(), &request, &bytes, FALSE);
#endif

	if (!result || (metadataBytes != bytes))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// Load seek tables (XMA2 / xWMA)
	const DWORD seekLen = m_header.Segments[HEADER::SEGIDX_SEEKTABLES].dwLength;
	if (seekLen > 0)
	{
		m_seekData.reset(new (std::nothrow) u0_t[seekLen]);
		if (!m_seekData)
			return E_OUTOFMEMORY;

		memset(&request, 0, sizeof(OVERLAPPED));
		request.Offset = m_header.Segments[HEADER::SEGIDX_SEEKTABLES].dwOffset;
		request.hEvent = m_event.get();

		wait = false;
		if (!ReadFile(hFile.get(), m_seekData.get(), seekLen, nullptr, &request))
		{
			const DWORD error = GetLastError();
			if (error != ERROR_IO_PENDING)
				return HRESULT_FROM_WIN32(error);
			wait = true;
		}

	#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		result = GetOverlappedResultEx(hFile.get(), &request, &bytes, INFINITE, FALSE);
	#else
		if (wait)
		{
			std::ignore = WaitForSingleObject(m_event.get(), INFINITE);
		}

		result = GetOverlappedResult(hFile.get(), &request, &bytes, FALSE);
	#endif

		if (!result || (seekLen != bytes))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (be)
		{
			auto ptr = reinterpret_cast<u2_t*>(m_seekData.get());
			for (size_t j = 0; j < seekLen; j += 4, ++ptr)
			{
				*ptr = _byteswap_ulong(*ptr);
			}
		}
	}

	const DWORD waveLen = m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength;
	if (!waveLen)
	{
		return HRESULT_FROM_WIN32(ERROR_NO_DATA);
	}

	if (m_data.dwFlags & BANKDATA::TYPE_STREAMING)
	{
		// If streaming, reopen without buffering
		hFile.reset();

	#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		CREATEFILE2_EXTENDED_PARAMETERS params2 = { sizeof(CREATEFILE2_EXTENDED_PARAMETERS), 0, 0, 0, {}, nullptr };
		params2.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		params2.dwFileFlags = FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING;
		m_async = CreateFile2(szFileName,
							  GENERIC_READ,
							  FILE_SHARE_READ,
							  OPEN_EXISTING,
							  &params2);
	#else
		m_async = CreateFileW(szFileName,
							  GENERIC_READ,
							  FILE_SHARE_READ,
							  nullptr,
							  OPEN_EXISTING,
							  FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
							  nullptr);
	#endif

		if (m_async == INVALID_HANDLE_VALUE)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		m_prepared = true;
	}
	else
	{
		// If in-memory, kick off read of wave data
		void* dest = nullptr;

		{
			m_waveData.reset(new (std::nothrow) u0_t[waveLen]);
			if (!m_waveData)
				return E_OUTOFMEMORY;

			dest = m_waveData.get();
		}

		memset(&m_request, 0, sizeof(OVERLAPPED));
		m_request.Offset = m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset;
		m_request.hEvent = m_event.get();

		if (!ReadFile(hFile.get(), dest, waveLen, nullptr, &m_request))
		{
			const DWORD error = GetLastError();
			if (error != ERROR_IO_PENDING)
				return HRESULT_FROM_WIN32(error);
		}
		else
		{
			m_prepared = true;
			memset(&m_request, 0, sizeof(OVERLAPPED));
		}

		m_async = hFile.release();
	}

	return S_OK;
}


void WaveBankReader::WaveBankReaderImpl::Close() noexcept
{
	if (m_async != INVALID_HANDLE_VALUE)
	{
		if (m_request.hEvent)
		{
			DWORD bytes;
		#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
			std::ignore = GetOverlappedResultEx(m_async, &m_request, &bytes, INFINITE, FALSE);
		#else
			std::ignore = WaitForSingleObject(m_request.hEvent, INFINITE);

			std::ignore = GetOverlappedResult(m_async, &m_request, &bytes, FALSE);
		#endif
		}

		CloseHandle(m_async);
		m_async = INVALID_HANDLE_VALUE;
	}
	m_event.reset();
}


_Use_decl_annotations_
HRESULT WaveBankReader::WaveBankReaderImpl::GetFormat(u2_t index, WAVEFORMATEX* pFormat, size_t maxsize) const noexcept
{
	if (!pFormat || !maxsize)
		return E_INVALIDARG;

	if (index >= m_data.dwEntryCount || !m_entries)
	{
		return E_FAIL;
	}

	auto& miniFmt = (m_data.dwFlags & BANKDATA::FLAGS_COMPACT) ? m_data.CompactFormat : (reinterpret_cast<const ENTRY*>(m_entries.get())[index].Format);

	switch (miniFmt.wFormatTag)
	{
		case MINIWAVEFORMAT::TAG_PCM:
			if (maxsize < sizeof(PCMWAVEFORMAT))
				return HRESULT_FROM_WIN32(ERROR_MORE_DATA);

			pFormat->wFormatTag = WAVE_FORMAT_PCM;

			if (maxsize >= sizeof(WAVEFORMATEX))
			{
				pFormat->cbSize = 0;
			}
			break;

		case MINIWAVEFORMAT::TAG_ADPCM:
			if (maxsize < (sizeof(WAVEFORMATEX) + 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/))
				return HRESULT_FROM_WIN32(ERROR_MORE_DATA);

			pFormat->wFormatTag = WAVE_FORMAT_ADPCM;
			pFormat->cbSize = 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/;
			{
				auto adpcmFmt = reinterpret_cast<ADPCMWAVEFORMAT*>(pFormat);
				adpcmFmt->wSamplesPerBlock = static_cast<WORD>(miniFmt.AdpcmSamplesPerBlock());
				miniFmt.AdpcmFillCoefficientTable(adpcmFmt);
			}
			break;

		case MINIWAVEFORMAT::TAG_WMA:
			if (maxsize < sizeof(WAVEFORMATEX))
				return HRESULT_FROM_WIN32(ERROR_MORE_DATA);

			pFormat->wFormatTag = static_cast<WORD>((miniFmt.wBitsPerSample & 0x1) ? WAVE_FORMAT_WMAUDIO3 : WAVE_FORMAT_WMAUDIO2);
			pFormat->cbSize = 0;
			break;

		case MINIWAVEFORMAT::TAG_XMA: // XMA2 is supported by Xbox One
			return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

		default:
			return E_FAIL;
	}

	pFormat->nChannels = miniFmt.nChannels;
	pFormat->wBitsPerSample = miniFmt.BitsPerSample();
	pFormat->nBlockAlign = static_cast<WORD>(miniFmt.BlockAlign());
	pFormat->nSamplesPerSec = miniFmt.nSamplesPerSec;
	pFormat->nAvgBytesPerSec = miniFmt.AvgBytesPerSec();

	return S_OK;
}


_Use_decl_annotations_
HRESULT WaveBankReader::WaveBankReaderImpl::GetWaveData(u2_t index, const u0_t** pData, u2_t& dataSize) const noexcept
{
	if (!pData)
		return E_INVALIDARG;

	if (index >= m_data.dwEntryCount || !m_entries)
	{
		return E_FAIL;
	}

	const u0_t* waveData = m_waveData.get();

	if (!waveData)
		return E_FAIL;

	if (m_data.dwFlags & BANKDATA::TYPE_STREAMING)
	{
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	}

	if (!m_prepared)
	{
		return HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
	}

	if (m_data.dwFlags & BANKDATA::FLAGS_COMPACT)
	{
		auto& entry = reinterpret_cast<const ENTRYCOMPACT*>(m_entries.get())[index];

		DWORD dwOffset, dwLength;
		entry.ComputeLocations(dwOffset, dwLength, index, m_header, m_data, reinterpret_cast<const ENTRYCOMPACT*>(m_entries.get()));

		if ((u3_t(dwOffset) + u3_t(dwLength)) > u3_t(m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength))
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}

		*pData = &waveData[dwOffset];
		dataSize = dwLength;
	}
	else
	{
		auto& entry = reinterpret_cast<const ENTRY*>(m_entries.get())[index];

		if ((u3_t(entry.PlayRegion.dwOffset) + u3_t(entry.PlayRegion.dwLength)) > u3_t(m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength))
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}

		*pData = &waveData[entry.PlayRegion.dwOffset];
		dataSize = entry.PlayRegion.dwLength;
	}

	return S_OK;
}


_Use_decl_annotations_
HRESULT WaveBankReader::WaveBankReaderImpl::GetSeekTable(u2_t index, const u2_t** pData, u2_t& dataCount, u2_t& tag) const noexcept
{
	if (!pData)
		return E_INVALIDARG;

	*pData = nullptr;
	dataCount = 0;
	tag = 0;

	if (index >= m_data.dwEntryCount || !m_entries)
	{
		return E_FAIL;
	}

	if (!m_seekData)
		return S_OK;

	auto& miniFmt = (m_data.dwFlags & BANKDATA::FLAGS_COMPACT) ? m_data.CompactFormat : (reinterpret_cast<const ENTRY*>(m_entries.get())[index].Format);

	switch (miniFmt.wFormatTag)
	{
		case MINIWAVEFORMAT::TAG_WMA:
			tag = static_cast<u2_t>((miniFmt.wBitsPerSample & 0x1) ? WAVE_FORMAT_WMAUDIO3 : WAVE_FORMAT_WMAUDIO2);
			break;

		case MINIWAVEFORMAT::TAG_XMA:
			tag = 0x166 /* WAVE_FORMAT_XMA2 */;
			break;

		default:
			return S_OK;
	}

	auto seekTable = FindSeekTable(index, m_seekData.get(), m_header, m_data);
	if (!seekTable)
		return S_OK;

	dataCount = *seekTable;
	*pData = seekTable + 1;

	return S_OK;
}


_Use_decl_annotations_
HRESULT WaveBankReader::WaveBankReaderImpl::GetMetadata(u2_t index, Metadata& metadata) const noexcept
{
	if (index >= m_data.dwEntryCount || !m_entries)
	{
		return E_FAIL;
	}

	if (m_data.dwFlags & BANKDATA::FLAGS_COMPACT)
	{
		auto& entry = reinterpret_cast<const ENTRYCOMPACT*>(m_entries.get())[index];

		DWORD dwOffset, dwLength;
		entry.ComputeLocations(dwOffset, dwLength, index, m_header, m_data, reinterpret_cast<const ENTRYCOMPACT*>(m_entries.get()));

		auto seekTable = FindSeekTable(index, m_seekData.get(), m_header, m_data);
		metadata.Duration = entry.GetDuration(dwLength, m_data, seekTable);
		metadata.LoopStart = metadata.LoopLength = 0;
		metadata.OffsetBytes = dwOffset;
		metadata.LengthBytes = dwLength;
	}
	else
	{
		auto& entry = reinterpret_cast<const ENTRY*>(m_entries.get())[index];

		metadata.Duration = entry.Duration;
		metadata.LoopStart = entry.LoopRegion.dwStartSample;
		metadata.LoopLength = entry.LoopRegion.dwTotalSamples;
		metadata.OffsetBytes = entry.PlayRegion.dwOffset;
		metadata.LengthBytes = entry.PlayRegion.dwLength;
	}

	if (m_data.dwFlags & BANKDATA::TYPE_STREAMING)
	{
		const u3_t offset = u3_t(metadata.OffsetBytes) + u3_t(m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwOffset);
		if (offset > UINT32_MAX)
			return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

		metadata.OffsetBytes = static_cast<u2_t>(offset);
	}

	return S_OK;
}


bool WaveBankReader::WaveBankReaderImpl::UpdatePrepared() noexcept
{
	if (m_prepared)
		return true;

	if (m_async == INVALID_HANDLE_VALUE)
		return false;

	if (m_request.hEvent)
	{

	#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		DWORD bytes;
		const BOOL result = GetOverlappedResultEx(m_async, &m_request, &bytes, 0, FALSE);
	#else
		const bool result = HasOverlappedIoCompleted(&m_request);
	#endif
		if (result)
		{
			m_prepared = true;

			memset(&m_request, 0, sizeof(OVERLAPPED));
		}
	}

	return m_prepared;
}



//--------------------------------------------------------------------------------------
WaveBankReader::WaveBankReader() noexcept(false) : pImpl(std::make_unique<WaveBankReaderImpl>()) { }
bool WaveBankReader::HasNames() const noexcept { return !pImpl->m_names.empty(); }
bool WaveBankReader::IsStreamingBank() const noexcept { return (pImpl->m_data.dwFlags  & BANKDATA::TYPE_STREAMING) != 0; }
const sc_t* WaveBankReader::BankName() const noexcept { return pImpl->m_data.szBankName; }
u2_t WaveBankReader::Count() const noexcept { return pImpl->m_data.dwEntryCount; }
u2_t WaveBankReader::BankAudioSize() const noexcept { return pImpl->m_header.Segments[HEADER::SEGIDX_ENTRYWAVEDATA].dwLength; }
_Use_decl_annotations_
HRESULT WaveBankReader::GetFormat(u2_t index, WAVEFORMATEX* pFormat, size_t maxsize) const noexcept { return pImpl->GetFormat(index, pFormat, maxsize); }
_Use_decl_annotations_
HRESULT WaveBankReader::GetWaveData(u2_t index, const u0_t** pData, u2_t& dataSize) const noexcept { return pImpl->GetWaveData(index, pData, dataSize); }
_Use_decl_annotations_
HRESULT WaveBankReader::GetSeekTable(u2_t index, const u2_t** pData, u2_t& dataCount, u2_t& tag) const noexcept { return pImpl->GetSeekTable(index, pData, dataCount, tag); }
_Use_decl_annotations_
HRESULT WaveBankReader::GetMetadata(u2_t index, Metadata& metadata) const noexcept { return pImpl->GetMetadata(index, metadata); }
HANDLE WaveBankReader::GetAsyncHandle() const noexcept { return (pImpl->m_data.dwFlags & BANKDATA::TYPE_STREAMING) ? pImpl->m_async : INVALID_HANDLE_VALUE; }
u2_t WaveBankReader::GetWaveAlignment() const noexcept { return pImpl->m_data.dwAlignment; }

_Use_decl_annotations_
HRESULT WaveBankReader::Open(const wchar_t* szFileName) noexcept { return pImpl->Open(szFileName); }

_Use_decl_annotations_
u2_t WaveBankReader::Find(const sc_t* name) const {
	auto it = pImpl->m_names.find(name);
	if (it != pImpl->m_names.cend())
		return it->second;

	return u2_t(-1);
}


bool WaveBankReader::IsPrepared() noexcept{
	if (pImpl->m_prepared)
		return true;

	return pImpl->UpdatePrepared();
}


void WaveBankReader::WaitOnPrepare() noexcept {
	if (pImpl->m_prepared)
		return;

	if (pImpl->m_request.hEvent) {
		std::ignore = WaitForSingleObjectEx(pImpl->m_request.hEvent, INFINITE, FALSE);
		pImpl->UpdatePrepared();
	}
}

#endif // GPK_WINDOWS - ignore the whole thing if not a Windows build
