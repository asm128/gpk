#include "gpk_enum.h"

#ifndef GPK_MATRIX_H_23627
#define GPK_MATRIX_H_23627

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(MATRIX_MATH, u0_t);
	GDEFINE_ENUM_VALUE(MATRIX_MATH, ROW_VECTOR		, 0);		
	GDEFINE_ENUM_VALUE(MATRIX_MATH, COLUMN_VECTOR	, 1);			

	GDEFINE_ENUM_TYPE(MATRIX_LAYOUT, u0_t);
	GDEFINE_ENUM_VALUE(MATRIX_LAYOUT, ROW_MAJOR		, 0);
	GDEFINE_ENUM_VALUE(MATRIX_LAYOUT, COLUMN_MAJOR	, 1);

	tplt<u0_t _width, MATRIX_LAYOUT _layout>
	cxpr u0_t	matrixIndex		(u0_t row, u0_t column) nxpt {
		rtrn (u0_t)(MATRIX_LAYOUT_ROW_MAJOR == _layout ? row * _width + column : column * _width + row);
	}

	tplt<tpnm T>
	stct m3 {
		T	Value[9]	= {};
	};

	tplt<u0_t _width, tpnm TMatrix>
	cxpr bool	matrixEqual		(cnst TMatrix & left, cnst TMatrix & right) nxpt {
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				if(left(row, column) != right(row, column)) {
					rtrn false;
				}
			}
		}
		rtrn true;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixAdd		(cnst TMatrix & left, cnst TMatrix & right) nxpt {
		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				result(row, column) = (typename TMatrix::TValue)(left(row, column) + right(row, column));
			}
		}
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixSubtract	(cnst TMatrix & left, cnst TMatrix & right) nxpt {
		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				result(row, column) = (typename TMatrix::TValue)(left(row, column) - right(row, column));
			}
		}
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixScale		(cnst TMatrix & matrix, f3_t scalar) nxpt {
		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				result(row, column) = (typename TMatrix::TValue)(matrix(row, column) * scalar);
			}
		}
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixMultiply	(cnst TMatrix & left, cnst TMatrix & right) nxpt {
		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				f3_t value = {};
				for(u0_t index = 0; index < _width; ++index) {
					value += left(row, index) * right(index, column);
				}
				result(row, column) = (typename TMatrix::TValue)value;
			}
		}
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixIdentity	() nxpt {
		TMatrix result = {};
		for(u0_t index = 0; index < _width; ++index)
			result(index, index) = 1;
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixTranspose	(cnst TMatrix & matrix) nxpt {
		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				result(row, column) = matrix(column, row);
			}
		}
		rtrn result;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr TMatrix	matrixInterpolate(cnst TMatrix & left, cnst TMatrix & right, f3_t factor) nxpt {
		rtrn matrixAdd<_width>(left, matrixScale<_width>(matrixSubtract<_width>(right, left), factor));
	}

	cxpr f3_t	matrixDeterminant(cnst f3_t (&values)[16], u0_t width) nxpt {
		if(1 == width)
			rtrn values[0];
		if(2 == width)
			rtrn values[0] * values[3] - values[1] * values[2];

		f3_t determinant = {};
		for(u0_t excludedColumn = 0; excludedColumn < width; ++excludedColumn) {
			f3_t minor[16] = {};
			u0_t index = {};
			for(u0_t row = 1; row < width; ++row) {
				for(u0_t column = 0; column < width; ++column) {
					if(column != excludedColumn) {
						minor[index++] = values[row * width + column];
					}
				}
			}
			determinant += (excludedColumn % 2 ? -1 : 1) * values[excludedColumn] * matrixDeterminant(minor, (u0_t)(width - 1));
		}
		rtrn determinant;
	}

	tplt<u0_t _width, tpnm TMatrix>
	cxpr f3_t	matrixDeterminant(cnst TMatrix & matrix) nxpt {
		f3_t values[16] = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				values[row * _width + column] = matrix(row, column);
			}
		}
		rtrn matrixDeterminant(values, _width);
	}

	tplt<u0_t _width, tpnm TMatrix>
	TMatrix	matrixInverse	(cnst TMatrix & matrix) {
		cnst f3_t determinant = matrixDeterminant<_width>(matrix);
		if(0 == determinant)
			rtrn matrixIdentity<_width, TMatrix>();

		TMatrix result = {};
		for(u0_t row = 0; row < _width; ++row) {
			for(u0_t column = 0; column < _width; ++column) {
				f3_t minor[16] = {};
				u0_t index = {};
				for(u0_t sourceRow = 0; sourceRow < _width; ++sourceRow) {
					for(u0_t sourceColumn = 0; sourceColumn < _width; ++sourceColumn) {
						if(sourceRow != row && sourceColumn != column) {
							minor[index++] = matrix(sourceRow, sourceColumn);
						}
					}
				}
				cnst f3_t cofactor = ((row + column) % 2 ? -1 : 1) * matrixDeterminant(minor, (u0_t)(_width - 1));
				result(column, row) = (typename TMatrix::TValue)(cofactor / determinant);
			}
		}
		rtrn result;
	}

	tydf m3<uc_t>	m3uc_t;	tdcs m3uc_t	m3uc_c;
	tydf m3<sc_t>	m3sc_t;	tdcs m3sc_t	m3sc_c;
	tydf m3<u0_t>	m3u0_t;	tdcs m3u0_t	m3u0_c;
	tydf m3<u1_t>	m3u1_t;	tdcs m3u1_t	m3u1_c;
	tydf m3<u2_t>	m3u2_t;	tdcs m3u2_t	m3u2_c;
	tydf m3<u3_t>	m3u3_t;	tdcs m3u3_t	m3u3_c;
	tydf m3<s0_t>	m3s0_t;	tdcs m3s0_t	m3s0_c;
	tydf m3<s1_t>	m3s1_t;	tdcs m3s1_t	m3s1_c;
	tydf m3<s2_t>	m3s2_t;	tdcs m3s2_t	m3s2_c;
	tydf m3<s3_t>	m3s3_t;	tdcs m3s3_t	m3s3_c;
	tydf m3<f2_t>	m3f2_t;	tdcs m3f2_t	m3f2_c;
	tydf m3<f3_t>	m3f3_t;	tdcs m3f3_t	m3f3_c;

#pragma pack(pop)
}

#endif // GPK_MATRIX_H_23627
