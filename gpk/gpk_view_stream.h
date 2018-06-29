#include "gpk_view_array.h"

#ifndef GPK_STREAM_VIEW_H_992834928749283
#define GPK_STREAM_VIEW_H_992834928749283

namespace gpk
{
	template<typename _tElement>
	struct view_stream : public view_array<_tElement> {
		uint32_t										CursorPosition					= 0;
		
		using											view_array<_tElement>::			view_array;
		using											view_array<_tElement>::			Data;

		//template<typename _tPOD>					inline	::gpk::error_t					rewd								(_tPOD& toSkip)					{ CursorPosition += sizeof(_tPOD); return CursorPosition; }
		//template<typename _tPOD>					inline	::gpk::error_t					ffwd								(_tPOD& toSkip)					{ CursorPosition += sizeof(_tPOD); return CursorPosition; }

		template<typename _tPOD>					inline	::gpk::error_t					read_pod							(_tPOD& read)					{ memcpy(&read, &Data[CursorPosition], sizeof(_tPOD));																						CursorPosition += sizeof(_tPOD);	return sizeof(_tPOD); }
		template<typename _tOBJ>					inline	::gpk::error_t					read_obj							(_tOBJ& read)					{ read							= *(_tOBJ*)&Data[CursorPosition];																			CursorPosition += sizeof(_tOBJ);	return sizeof(_tOBJ); }
		template<typename _tPOD>					inline	::gpk::error_t					read_pod							(_tPOD* read, uint32_t nCount)	{ const int32_t						sizeToCopy = sizeof(_tPOD) * nCount ;				memcpy(read, &Data[CursorPosition], sizeToCopy);	CursorPosition += sizeToCopy;		return sizeToCopy; }
		template<typename _tPOD, size_t _sCount>	inline	::gpk::error_t					read_pod							(_tPOD (&read)[_sCount])		{ static constexpr const int32_t	sizeToCopy = (int32_t)(sizeof(_tPOD) * _sCount);	memcpy(read, &Data[CursorPosition], sizeToCopy);	CursorPosition += sizeToCopy;		return sizeToCopy; }
		template<typename _tOBJ>							::gpk::error_t					read_obj							(_tOBJ* read, uint32_t nCount)	{ 
			_tOBJ																					* start								= (_tOBJ*)&Data[CursorPosition];
			for(uint32_t i = 0; i < nCount; ++i)
				read[i]																				= start[i]; 
			return sizeof(_tOBJ) * nCount; 
		}
	};
} // namespace

#endif // GPK_STREAM_VIEW_H_992834928749283
