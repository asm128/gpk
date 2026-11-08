#include "gpk_matrix3.h"

#ifndef GPK_MODEL_H_23627
#define GPK_MODEL_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	struct SModelMatrices {
		m4f2_t	Scale		;
		m4f2_t	Rotation	;
		m4f2_t	Position	;
	};

	struct SModel3 {
		n3f2_t	Scale		;
		n3f2_t	Rotation	;
		n3f2_t	Position	;
	};
#pragma pack(pop)
} // nmsp

#endif // GPK_MODEL_H_23627
