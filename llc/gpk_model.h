#include "gpk_matrix.h"

#ifndef GPK_MODEL_H_23627
#define GPK_MODEL_H_23627

namespace gpk
{
#pragma pack(push, 1)
	struct SModelMatrices {
		::gpk::m4f32	Scale		;
		::gpk::m4f32	Rotation	;
		::gpk::m4f32	Position	;
	};

	struct SModel3 {
		::gpk::n3f2_t	Scale		;
		::gpk::n3f2_t	Rotation	;
		::gpk::n3f2_t	Position	;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_MODEL_H_23627
