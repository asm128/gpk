#include "gpk_view.h"

#ifndef GPK_STREAM_VIEW_H_23627
#define GPK_STREAM_VIEW_H_23627

namespace gpk
{
	tplt<tpnm _tElement>
	struct view_stream : public view<_tElement> {
															uint32_t						CursorPosition						= 0;

		using												view<_tElement>::				view;
		using												view<_tElement>::				Data;

		//tplt<tpnm _tPOD>					inline	::gpk::error_t					rewd								(_tPOD& toSkip)							{ CursorPosition += sizeof(_tPOD); return CursorPosition; }
		//tplt<tpnm _tPOD>					inline	::gpk::error_t					ffwd								(_tPOD& toSkip)							{ CursorPosition += sizeof(_tPOD); return CursorPosition; }

		constexpr									inline	uint32_t						remaining							()								const	{ return this->size() - CursorPosition; }

		tplt<tpnm _tPOD>					inline	::gpk::error_t					read_pod							(_tPOD& read)							{ ree_if(CursorPosition + sizeof(_tPOD) > this->size(), "Out of range. Max size: %u.", this->size()); memcpy(&read, &Data[CursorPosition], sizeof(_tPOD));	CursorPosition += sizeof(_tPOD);	return sizeof(_tPOD); }
		tplt<tpnm _tOBJ>					inline	::gpk::error_t					read_obj							(_tOBJ& read)							{ ree_if(CursorPosition + sizeof(_tOBJ) > this->size(), "Out of range. Max size: %u.", this->size()); read = *(_tOBJ*)&Data[CursorPosition];				CursorPosition += sizeof(_tOBJ);	return sizeof(_tOBJ); }
		tplt<tpnm _tPOD>					inline	::gpk::error_t					read_pod							(_tPOD* read, uint32_t nCount)			{ const int32_t						sizeToCopy = sizeof(_tPOD) * nCount;				memcpy(read, &Data[CursorPosition], sizeToCopy);		CursorPosition += sizeToCopy;		return nCount; }
		tplt<tpnm _tPOD, size_t _sCount>	inline	::gpk::error_t					read_pod							(_tPOD (&read)[_sCount])				{ stacxpr	const int32_t	sizeToCopy = (int32_t)(sizeof(_tPOD) * _sCount);	memcpy(read, &Data[CursorPosition], sizeToCopy);		CursorPosition += sizeToCopy;		return _sCount; }
		tplt<tpnm _tOBJ>							::gpk::error_t					read_obj							(_tOBJ* read, uint32_t nCount)			{
			_tOBJ				* start								= (_tOBJ*)&Data[CursorPosition];
			for(uint32_t i = 0; i < nCount; ++i)
				read[i]			= start[i];
			return sizeof(_tOBJ) * nCount;
		}

		tplt<tpnm _tPOD>					inline	::gpk::error_t					write_pod			(const _tPOD& towrite)					{ ree_if(CursorPosition + sizeof(_tPOD) >= this->size(), "Out of range. Max size: %u.", this->size()); memcpy(&Data[CursorPosition], &towrite, sizeof(_tPOD));	CursorPosition += sizeof(_tPOD);	return sizeof(_tPOD); }
		tplt<tpnm _tOBJ>					inline	::gpk::error_t					write_obj			(const _tOBJ& towrite)					{ ree_if(CursorPosition + sizeof(_tOBJ) >= this->size(), "Out of range. Max size: %u.", this->size()); *(_tOBJ*)&Data[CursorPosition]	= towrite;				CursorPosition += sizeof(_tOBJ);	return sizeof(_tOBJ); }
		tplt<tpnm _tPOD>					inline	::gpk::error_t					write_pod			(const _tPOD* towrite, uint32_t nCount)	{ const int32_t						sizeToCopy = sizeof(_tPOD) * nCount ;				memcpy(&Data[CursorPosition], towrite, sizeToCopy);	CursorPosition += sizeToCopy;		return sizeToCopy; }
		tplt<tpnm _tPOD, size_t _sCount>	inline	::gpk::error_t					write_pod			(const _tPOD (&towrite)[_sCount])		{ stacxpr	const int32_t	sizeToCopy = (int32_t)(sizeof(_tPOD) * _sCount);	memcpy(&Data[CursorPosition], towrite, sizeToCopy);	CursorPosition += sizeToCopy;		return sizeToCopy; }
		tplt<tpnm _tOBJ>							::gpk::error_t					write_obj			(const _tOBJ* towrite, uint32_t nCount)	{
			_tOBJ				* start								= (_tOBJ*)&Data[CursorPosition];
			for(uint32_t i = 0; i < nCount; ++i)
				start[i]																			= towrite[i];
			return sizeof(_tOBJ) * nCount;
		}
	};
} // namespace

#endif // GPK_STREAM_VIEW_H_23627
