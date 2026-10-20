#include "gpk_matrix.h"
#include "gpk_n2.h"

#ifndef GPK_MATRIX2_H_23627
#define GPK_MATRIX2_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T, MATRIX_MATH _math = MATRIX_MATH_ROW_VECTOR, MATRIX_LAYOUT _layout = MATRIX_LAYOUT_ROW_MAJOR>
	stct m2 {
		tydf T							TValue;
		tydf m2<T, _math, _layout>	TMatrix;
		tydf n2<T>					TCoord;

		T	Value[4]	= {};

		cxpr T      &	oper()		(u0_t row, u0_t column)			nxpt	{ rtrn Value[matrixIndex<2, _layout>(row, column)]; }
		cxpr cnst T &	oper()		(u0_t row, u0_t column)	csnx	{ rtrn Value[matrixIndex<2, _layout>(row, column)]; }
		cxpr T      &	MathElement	(u0_t row, u0_t column)			nxpt	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }
		cxpr cnst T &	MathElement	(u0_t row, u0_t column)	csnx	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }

		cxpr bool	oper==	(cnst TMatrix & other)	csnx	{ rtrn matrixEqual<2>(*this, other); }
		cxpr bool	oper!=	(cnst TMatrix & other)	csnx	{ rtrn false == operator==(other); }
		cxpr TMatrix	oper+	(cnst TMatrix & other)	csnx	{ rtrn matrixAdd<2>(*this, other); }
		cxpr TMatrix	oper-	(cnst TMatrix & other)	csnx	{ rtrn matrixSubtract<2>(*this, other); }
		cxpr TMatrix	oper*	(f3_t scalar)			csnx	{ rtrn matrixScale<2>(*this, scalar); }
		     TMatrix	oper/	(f3_t scalar)			cnst		{ rtrn matrixScale<2>(*this, 1.0 / scalar); }
		cxpr TMatrix	oper*	(cnst TMatrix & other)	csnx	{ rtrn matrixMultiply<2>(*this, other); }

		TMatrix &	oper+=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this + other; }
		TMatrix &	oper-=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this - other; }
		TMatrix &	oper*=	(f3_t scalar)				nxpt	{ rtrn *this = *this * scalar; }
		TMatrix &	oper/=	(f3_t scalar)						{ rtrn *this = *this / scalar; }
		TMatrix &	oper*=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this * other; }

		stxp TMatrix	GetIdentity	() nxpt	{ rtrn matrixIdentity<2, TMatrix>(); }
		cxpr TMatrix	GetTranspose	() csnx	{ rtrn matrixTranspose<2>(*this); }
		     TMatrix	GetInverse	() cnst		{ rtrn matrixInverse<2>(*this); }
		cxpr f3_t	GetDeterminant	() csnx	{ rtrn matrixDeterminant<2>(*this); }

		void	Identity		() nxpt	{ *this = GetIdentity(); }
		void	Transpose		(cnst TMatrix & other) nxpt	{ *this = other.GetTranspose(); }
		void	Invert			()					{ *this = GetInverse(); }
		TMatrix & LinearInterpolate(cnst TMatrix & first, cnst TMatrix & second, f3_t factor) nxpt	{ rtrn *this = matrixInterpolate<2>(first, second, factor); }

		TCoord	Transform	(cnst TCoord & value) csnx {
			if(MATRIX_MATH_ROW_VECTOR == _math)
				rtrn {(T)(value.x * (*this)(0, 0) + value.y * (*this)(1, 0)), (T)(value.x * (*this)(0, 1) + value.y * (*this)(1, 1))};
			rtrn {(T)((*this)(0, 0) * value.x + (*this)(0, 1) * value.y), (T)((*this)(1, 0) * value.x + (*this)(1, 1) * value.y)};
		}
		TCoord	TransformInverse(cnst TCoord & value) csnx	{ rtrn GetTranspose().Transform(value); }

		void	Rotation	(f3_t angle) nxpt {
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 1) = (T)angleSinCos.Sin;
			MathElement(1, 0) = (T)-angleSinCos.Sin;	MathElement(1, 1) = (T)angleSinCos.Cos;
		}
		void	Scale		(cnst TCoord & scale, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(0, 0) = (T)(MathElement(0, 0) * scale.x);
			MathElement(1, 1) = (T)(MathElement(1, 1) * scale.y);
		}
	};

	tplt<tpnm T, MATRIX_MATH _math = MATRIX_MATH_ROW_VECTOR, MATRIX_LAYOUT _layout = MATRIX_LAYOUT_ROW_MAJOR>
	stct m3a2 : m3<T> {
		tydf T							TValue;
		tydf m3<T>					TStorage;
		tydf m3a2<T, _math, _layout>	TMatrix;
		tydf n2<T>					TCoord;
		usng TStorage::Value;

		cxpr T      &	oper()		(u0_t row, u0_t column)			nxpt	{ rtrn Value[matrixIndex<3, _layout>(row, column)]; }
		cxpr cnst T &	oper()		(u0_t row, u0_t column)	csnx	{ rtrn Value[matrixIndex<3, _layout>(row, column)]; }
		cxpr T      &	MathElement	(u0_t row, u0_t column)			nxpt	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }
		cxpr cnst T &	MathElement	(u0_t row, u0_t column)	csnx	{ rtrn MATRIX_MATH_ROW_VECTOR == _math ? (*this)(row, column) : (*this)(column, row); }

		cxpr bool	oper==	(cnst TMatrix & other)	csnx	{ rtrn matrixEqual<3>(*this, other); }
		cxpr bool	oper!=	(cnst TMatrix & other)	csnx	{ rtrn false == operator==(other); }
		cxpr TMatrix	oper+	(cnst TMatrix & other)	csnx	{ rtrn matrixAdd<3>(*this, other); }
		cxpr TMatrix	oper-	(cnst TMatrix & other)	csnx	{ rtrn matrixSubtract<3>(*this, other); }
		cxpr TMatrix	oper*	(f3_t scalar)			csnx	{ rtrn matrixScale<3>(*this, scalar); }
		     TMatrix	oper/	(f3_t scalar)			cnst		{ rtrn matrixScale<3>(*this, 1.0 / scalar); }
		cxpr TMatrix	oper*	(cnst TMatrix & other)	csnx	{ rtrn matrixMultiply<3>(*this, other); }

		TMatrix &	oper+=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this + other; }
		TMatrix &	oper-=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this - other; }
		TMatrix &	oper*=	(f3_t scalar)				nxpt	{ rtrn *this = *this * scalar; }
		TMatrix &	oper/=	(f3_t scalar)						{ rtrn *this = *this / scalar; }
		TMatrix &	oper*=	(cnst TMatrix & other)	nxpt	{ rtrn *this = *this * other; }

		stxp TMatrix	GetIdentity	() nxpt	{ rtrn matrixIdentity<3, TMatrix>(); }
		cxpr TMatrix	GetTranspose	() csnx	{ rtrn matrixTranspose<3>(*this); }
		     TMatrix	GetInverse	() cnst		{ rtrn matrixInverse<3>(*this); }
		cxpr f3_t	GetDeterminant	() csnx	{ rtrn matrixDeterminant<3>(*this); }
		cxpr TCoord	GetTranslation	() csnx	{ rtrn {MathElement(2, 0), MathElement(2, 1)}; }

		void	Identity		() nxpt	{ *this = GetIdentity(); }
		void	Transpose		(cnst TMatrix & other) nxpt	{ *this = other.GetTranspose(); }
		void	Invert			()					{ *this = GetInverse(); }
		TMatrix & LinearInterpolate(cnst TMatrix & first, cnst TMatrix & second, f3_t factor) nxpt	{ rtrn *this = matrixInterpolate<3>(first, second, factor); }

		TCoord	TransformPoint(cnst TCoord & value) csnx {
			f3_t x = {}, y = {}, w = {};
			if(MATRIX_MATH_ROW_VECTOR == _math) {
				x = value.x * (*this)(0, 0) + value.y * (*this)(1, 0) + (*this)(2, 0);
				y = value.x * (*this)(0, 1) + value.y * (*this)(1, 1) + (*this)(2, 1);
				w = value.x * (*this)(0, 2) + value.y * (*this)(1, 2) + (*this)(2, 2);
			}
			else {
				x = (*this)(0, 0) * value.x + (*this)(0, 1) * value.y + (*this)(0, 2);
				y = (*this)(1, 0) * value.x + (*this)(1, 1) * value.y + (*this)(1, 2);
				w = (*this)(2, 0) * value.x + (*this)(2, 1) * value.y + (*this)(2, 2);
			}
			rtrn {(T)(x / w), (T)(y / w)};
		}
		TCoord	TransformDirection(cnst TCoord & value) csnx {
			if(MATRIX_MATH_ROW_VECTOR == _math)
				rtrn {(T)(value.x * (*this)(0, 0) + value.y * (*this)(1, 0)), (T)(value.x * (*this)(0, 1) + value.y * (*this)(1, 1))};
			rtrn {(T)((*this)(0, 0) * value.x + (*this)(0, 1) * value.y), (T)((*this)(1, 0) * value.x + (*this)(1, 1) * value.y)};
		}

		void	Rotation	(f3_t angle) nxpt {
			Identity();
			cnst SSinCos angleSinCos = getSinCos(angle);
			MathElement(0, 0) = (T)angleSinCos.Cos;	MathElement(0, 1) = (T)angleSinCos.Sin;
			MathElement(1, 0) = (T)-angleSinCos.Sin;	MathElement(1, 1) = (T)angleSinCos.Cos;
		}
		void	Scale		(cnst TCoord & scale, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(0, 0) = (T)(MathElement(0, 0) * scale.x);
			MathElement(1, 1) = (T)(MathElement(1, 1) * scale.y);
		}
		void	SetTranslation(cnst TCoord & translation, bool eraseContent) nxpt {
			if(eraseContent)
				Identity();
			MathElement(2, 0) = translation.x;
			MathElement(2, 1) = translation.y;
		}
	};

	tydf m2<uc_t>	m2uc_t;	tdcs m2uc_t	m2uc_c;
	tydf m2<sc_t>	m2sc_t;	tdcs m2sc_t	m2sc_c;
	tydf m2<u0_t>	m2u0_t;	tdcs m2u0_t	m2u0_c;
	tydf m2<u1_t>	m2u1_t;	tdcs m2u1_t	m2u1_c;
	tydf m2<u2_t>	m2u2_t;	tdcs m2u2_t	m2u2_c;
	tydf m2<u3_t>	m2u3_t;	tdcs m2u3_t	m2u3_c;
	tydf m2<s0_t>	m2s0_t;	tdcs m2s0_t	m2s0_c;
	tydf m2<s1_t>	m2s1_t;	tdcs m2s1_t	m2s1_c;
	tydf m2<s2_t>	m2s2_t;	tdcs m2s2_t	m2s2_c;
	tydf m2<s3_t>	m2s3_t;	tdcs m2s3_t	m2s3_c;
	tydf m2<f2_t>	m2f2_t;	tdcs m2f2_t	m2f2_c;
	tydf m2<f3_t>	m2f3_t;	tdcs m2f3_t	m2f3_c;

	tydf m3a2<uc_t>	m3a2uc_t;	tdcs m3a2uc_t	m3a2uc_c;
	tydf m3a2<sc_t>	m3a2sc_t;	tdcs m3a2sc_t	m3a2sc_c;
	tydf m3a2<u0_t>	m3a2u0_t;	tdcs m3a2u0_t	m3a2u0_c;
	tydf m3a2<u1_t>	m3a2u1_t;	tdcs m3a2u1_t	m3a2u1_c;
	tydf m3a2<u2_t>	m3a2u2_t;	tdcs m3a2u2_t	m3a2u2_c;
	tydf m3a2<u3_t>	m3a2u3_t;	tdcs m3a2u3_t	m3a2u3_c;
	tydf m3a2<s0_t>	m3a2s0_t;	tdcs m3a2s0_t	m3a2s0_c;
	tydf m3a2<s1_t>	m3a2s1_t;	tdcs m3a2s1_t	m3a2s1_c;
	tydf m3a2<s2_t>	m3a2s2_t;	tdcs m3a2s2_t	m3a2s2_c;
	tydf m3a2<s3_t>	m3a2s3_t;	tdcs m3a2s3_t	m3a2s3_c;
	tydf m3a2<f2_t>	m3a2f2_t;	tdcs m3a2f2_t	m3a2f2_c;
	tydf m3a2<f3_t>	m3a2f3_t;	tdcs m3a2f3_t	m3a2f3_c;

#pragma pack(pop)
}

#endif // GPK_MATRIX2_H_23627


