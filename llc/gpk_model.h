#include "gpk_matrix.h"

#ifndef GPK_MODEL_H_2983472398
#define GPK_MODEL_H_2983472398

namespace gpk
{
#pragma pack(push, 1)
	struct SModelMatrices {
		::gpk::SMatrix4<float>								Scale		;
		::gpk::SMatrix4<float>								Rotation	;
		::gpk::SMatrix4<float>								Position	;
	};

	struct SModel3 {
		::gpk::n3<float>								Scale		;
		::gpk::n3<float>								Rotation	;
		::gpk::n3<float>								Position	;
	};

	struct SCamera {
		::gpk::n3<float>								Position			= {-0.000001f, 100, 0};
		::gpk::n3<float>								Target				= {};
		::gpk::n3<float>								Up					= {0, 1, 0};
	};
#pragma pack(pop)
} // namespace

#endif // GPK_MODEL_H_2983472398
