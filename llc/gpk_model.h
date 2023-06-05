#include "gpk_matrix.h"

#ifndef GPK_MODEL_H_2983472398
#define GPK_MODEL_H_2983472398

namespace gpk
{
#pragma pack(push, 1)
	struct SModelMatrices {
		::gpk::m4f32	Scale		;
		::gpk::m4f32	Rotation	;
		::gpk::m4f32	Position	;
	};

	struct SModel3 {
		::gpk::n3f32	Scale		;
		::gpk::n3f32	Rotation	;
		::gpk::n3f32	Position	;
	};

	//struct SCamera {
	//	::gpk::n3f32	Position	= {-0.000001f, 100, 0};
	//	::gpk::n3f32	Target		= {};
	//	::gpk::n3f32	Up			= {0, 1, 0};
	//};
#pragma pack(pop)
} // namespace

#endif // GPK_MODEL_H_2983472398
