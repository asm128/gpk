#include "gpk_n2.h"

#ifndef GPK_N3_H_23627
#define GPK_N3_H_23627

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tBase>
	struct n3 {
		typedef	_tBase		T;
		T					x, y, z;

		typedef	n3<T>		Tn3;
		typedef	n2<T>		Tn2;

		stincxp	Tn3			from				(T value)									noexcept	{ return {value, value, value}; }

		//
		GPK_DEFAULT_OPERATOR(Tn3, x == other.x && y == other.y && z == other.z);

		//
		cnstxpr	Tn3			operator+			(const Tn3 & other)					const	noexcept	{ return {x + other.x, y + other.y, z + other.z}; }
		cnstxpr	Tn3			operator-			(const Tn3 & other)					const	noexcept	{ return {x - other.x, y - other.y, z - other.z}; }
		cnstxpr	Tn3			operator*			(double scalar)						const	noexcept	{ return {T(x * scalar), T(y * scalar), T(z * scalar)}; }
		cnstxpr	Tn3			operator/			(double scalar)						const				{ return {T(x / scalar), T(y / scalar), T(z / scalar)}; }
		cnstxpr	Tn3			operator*			(int64_t scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar), T(z * scalar)}; }
		cnstxpr	Tn3			operator/			(int64_t scalar)					const				{ return {T(x / scalar), T(y / scalar), T(z / scalar)}; }
		cnstxpr	Tn3			operator*			(uint64_t scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar), T(z * scalar)}; }
		cnstxpr	Tn3			operator/			(uint64_t scalar)					const				{ return {T(x / scalar), T(y / scalar), T(z / scalar)}; }
		cnstxpr	Tn3			operator*			(int32_t scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar), T(z * scalar)}; }
		cnstxpr	Tn3			operator/			(int32_t scalar)					const				{ return {T(x / scalar), T(y / scalar), T(z / scalar)}; }
		cnstxpr	Tn3			operator*			(uint32_t scalar)					const	noexcept	{ return {T(x * scalar), T(y * scalar), T(z * scalar)}; }
		cnstxpr	Tn3			operator/			(uint32_t scalar)					const				{ return {T(x / scalar), T(y / scalar), T(z / scalar)}; }
		//
		Tn3&				operator+=			(const Tn3 & other)							noexcept	{ x += other.x; y += other.y; z += other.z;	return *this; }
		Tn3&				operator-=			(const Tn3 & other)							noexcept	{ x -= other.x; y -= other.y; z -= other.z;	return *this; }
		Tn3&				operator*=			(double scalar)								noexcept	{ x = T(x * scalar); y = T(y * scalar); z = T(z * scalar); return *this; }
		Tn3&				operator/=			(double scalar)											{ x = T(x / scalar); y = T(y / scalar); z = T(z / scalar); return *this; }
		Tn3&				operator*=			(int64_t scalar)							noexcept	{ x = T(x * scalar); y = T(y * scalar); z = T(z * scalar); return *this; }
		Tn3&				operator/=			(int64_t scalar)										{ x = T(x / scalar); y = T(y / scalar); z = T(z / scalar); return *this; }
		Tn3&				operator*=			(int32_t scalar)							noexcept	{ x = T(x * scalar); y = T(y * scalar); z = T(z * scalar); return *this; }
		Tn3&				operator/=			(int32_t scalar)										{ x = T(x / scalar); y = T(y / scalar); z = T(z / scalar); return *this; }
		Tn3&				operator*=			(uint32_t scalar)							noexcept	{ x = T(x * scalar); y = T(y * scalar); z = T(z * scalar); return *this; }
		Tn3&				operator/=			(uint32_t scalar)										{ x = T(x / scalar); y = T(y / scalar); z = T(z / scalar); return *this; }
		Tn3&				operator*=			(uint64_t scalar)							noexcept	{ x = T(x * scalar); y = T(y * scalar); z = T(z * scalar); return *this; }
		Tn3&				operator/=			(uint64_t scalar)										{ x = T(x / scalar); y = T(y / scalar); z = T(z / scalar); return *this; }
		//
		cnstxpr	Tn3			operator-			()									const	noexcept	{ return {T(x*-1), T(y*-1), T(z*-1)}; }
		//
		inlcxpr	Tn2			xx					()									const	noexcept	{ return {x, x}; }
		inlcxpr	Tn2			xy					()									const	noexcept	{ return {x, y}; }
		inlcxpr	Tn2			xz					()									const	noexcept	{ return {x, z}; }
		inlcxpr	Tn2			yx					()									const	noexcept	{ return {y, x}; }
		inlcxpr	Tn2			yy					()									const	noexcept	{ return {y, y}; }
		inlcxpr	Tn2			yz					()									const	noexcept	{ return {y, z}; }
		inlcxpr	Tn2			zx					()									const	noexcept	{ return {z, x}; }
		inlcxpr	Tn2			zy					()									const	noexcept	{ return {z, y}; }
		inlcxpr	Tn2			zz					()									const	noexcept	{ return {z, z}; }

		inlcxpr	Tn3			xxx					()									const	noexcept	{ return {x, x, x}; }
		inlcxpr	Tn3			xxy					()									const	noexcept	{ return {x, x, y}; }
		inlcxpr	Tn3			xxz					()									const	noexcept	{ return {x, x, z}; }
		inlcxpr	Tn3			xyx					()									const	noexcept	{ return {x, y, x}; }
		inlcxpr	Tn3			xyy					()									const	noexcept	{ return {x, y, y}; }
		inlcxpr	Tn3			xyz					()									const	noexcept	{ return {x, y, z}; }
		inlcxpr	Tn3			xzx					()									const	noexcept	{ return {x, z, x}; }
		inlcxpr	Tn3			xzy					()									const	noexcept	{ return {x, z, y}; }
		inlcxpr	Tn3			xzz					()									const	noexcept	{ return {x, z, z}; }

		inlcxpr	Tn3			yxx					()									const	noexcept	{ return {y, x, x}; }
		inlcxpr	Tn3			yxy					()									const	noexcept	{ return {y, x, y}; }
		inlcxpr	Tn3			yxz					()									const	noexcept	{ return {y, x, z}; }
		inlcxpr	Tn3			yyx					()									const	noexcept	{ return {y, y, x}; }
		inlcxpr	Tn3			yyy					()									const	noexcept	{ return {y, y, y}; }
		inlcxpr	Tn3			yyz					()									const	noexcept	{ return {y, y, z}; }
		inlcxpr	Tn3			yzx					()									const	noexcept	{ return {y, z, x}; }
		inlcxpr	Tn3			yzy					()									const	noexcept	{ return {y, z, y}; }
		inlcxpr	Tn3			yzz					()									const	noexcept	{ return {y, z, z}; }

		inlcxpr	Tn3			zxx					()									const	noexcept	{ return {z, x, x}; }
		inlcxpr	Tn3			zxy					()									const	noexcept	{ return {z, x, y}; }
		inlcxpr	Tn3			zxz					()									const	noexcept	{ return {z, x, z}; }
		inlcxpr	Tn3			zyx					()									const	noexcept	{ return {z, y, x}; }
		inlcxpr	Tn3			zyy					()									const	noexcept	{ return {z, y, y}; }
		inlcxpr	Tn3			zyz					()									const	noexcept	{ return {z, y, z}; }
		inlcxpr	Tn3			zzx					()									const	noexcept	{ return {z, z, x}; }
		inlcxpr	Tn3			zzy					()									const	noexcept	{ return {z, z, y}; }
		inlcxpr	Tn3			zzz					()									const	noexcept	{ return {z, z, z}; }

		tplt<tpnm _t2>
		inlcxpr	n3<_t2>		Cast				()									const	noexcept	{ return {(_t2)x, (_t2)y, (_t2)z}; }

		inlcxpr	n3<uint8_t>	u0_t					()									const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	n3<uint16_t>u1_t					()									const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	n3<uint32_t>u2_t					()									const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	n3<uint64_t>u3_t					()									const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	n3<int8_t>	s0_t					()									const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	n3<int16_t>	s1_t					()									const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	n3<int32_t>	s2_t					()									const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	n3<int64_t>	s3_t					()									const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	n3<float>	f2_t					()									const	noexcept	{ return Cast<float		>(); }
		inlcxpr	n3<double>	f3_t					()									const	noexcept	{ return Cast<double	>(); }

		//
		inlcxpr	Tn3&		Area				()									const	noexcept	{ return x * y * z; }
		inlcxpr	Tn3			Clamp				(const Tn3 & min, const Tn3 & max)	const	noexcept	{ return {::gpk::clamped(x, min.x, max.x), ::gpk::clamped(y, min.y, max.y), ::gpk::clamped(z, min.z, max.z)}; }
		inline	Tn3&		Set					(T value)									noexcept	{ x = y = z = value; return *this; }
		inline	Tn3&		From				(T value)									noexcept	{ x = y = z = value; return *this; }

		int					SetAxis				(uint32_t axisIndex, T value) {
			switch(axisIndex) { 
			case 0: x = value; return 0; 
			case 1: y = value; return 1; 
			case 2: z = value; return 2; 
			}
			return -1;
		}
		int					SetOthers			(uint32_t axisExcluded, T value) {
			switch(axisExcluded) { 
			case 0: y = z = value; break; 
			case 1: x = z = value; break; 
			case 2: x = y = value; break; 
			}
			return -1;
		}

		inline	Tn3&		Scale				(double scalar)								noexcept	{ return *this *= scalar; }
		inline	Tn3&		Scale				(const Tn3 & other)							noexcept	{ x *= other.x; y *= other.y; z *= other.z; return *this; }
		inlcxpr	Tn3&		Normalize			()														{ const T sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this; }

		inline	Tn3			Scaled				(double scalar)						const	noexcept	{ return Tn3{*this}.Scale(scalar); }
		inline	Tn3			Scaled				(const Tn3 & other)					const	noexcept	{ return {x * other.x, y * other.y, z * other.z }; }
		inlcxpr	Tn3			Normalized			()									const				{ const T sqLen = LengthSquared(); return sqLen ? *this / ::sqrt(sqLen) : *this; }

		cnstxpr	double		Dot					(const Tn3 & other)					const	noexcept	{ return x * other.x + y * other.y + z * other.z; }
		cnstxpr	T			LengthSquared		()									const	noexcept	{ return x * x + y * y + z * z; }
		cnstxpr	double		Length				()									const				{ const T sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0; }
		cnstxpr	double		AngleWith			(const Tn3 & other)					const				{ const double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0; }
		void				AddScaled			(const Tn3 & toScaleAndAdd, double scale)	noexcept	{
			x					+= (T)(toScaleAndAdd.x * scale);
			y					+= (T)(toScaleAndAdd.y * scale);
			z					+= (T)(toScaleAndAdd.z * scale);
		}
		cnstxpr	Tn3			Reflect				(const Tn3 & direction)				const	noexcept	{ return direction - *this * 2 * Dot(direction);	}
		cnstxpr	Tn3			Cross				(const Tn3 & right)					const	noexcept	{ return {y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x };	}
				Tn3&		Cross				(const Tn3 & vector1, const Tn3 & vector2)	noexcept	{
			x					= vector1.y * vector2.z - vector1.z * vector2.y;
			y					= vector1.z * vector2.x - vector1.x * vector2.z;
			z					= vector1.x * vector2.y - vector1.y * vector2.x;
			return *this;
		}
		Tn3&				CrossAndNormalize	(const Tn3 & in_vLeft, const Tn3 & in_vRight)			{
			x					= in_vLeft.y * in_vRight.z - in_vLeft.z * in_vRight.y;
			y					= in_vLeft.z * in_vRight.x - in_vLeft.x * in_vRight.z;
			z					= in_vLeft.x * in_vRight.y - in_vLeft.y * in_vRight.x;
			Normalize();
			return *this;
		};

		// TODO: Likely Y and Z are inverted. Please fix.
		Tn3&				RotateX				(double theta)											{
			const ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			const double			pz					= y * pairSinCos.Cos - z * pairSinCos.Sin;
			y					= (T)(y * pairSinCos.Sin + z * pairSinCos.Cos);
			z					= (T)pz;
			return *this;
		}

		Tn3&				RotateY				(double theta)											{
			const ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			const double			px					= x * pairSinCos.Cos - z * pairSinCos.Sin;
			z					= (T)(x * pairSinCos.Sin + z * pairSinCos.Cos);
			x					= (T)px;
			return *this;
		}

		Tn3&				RotateZ				(double theta)											{
			const ::gpk::SSinCos	pairSinCos			= ::gpk::getSinCos(theta);
			const double			px					= x * pairSinCos.Cos - y * pairSinCos.Sin;
			y					= (T)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x					= (T)px;
			return *this;
		}
	};	// struct n3
	typedef	n3<sc_t>		n3char;
	typedef	n3<uc_t>		n3uchar;
	typedef	n3<float>		n3f32;
	typedef	n3<double>		n3f64;
	typedef	n3<uint8_t>		n3u8;
	typedef	n3<uint16_t>	n3u16;
	typedef	n3<uint32_t>	n3u32;
	typedef	n3<uint64_t>	n3u64;
	typedef	n3<int8_t>		n3i8;
	typedef	n3<int16_t>		n3i16;
	typedef	n3<int32_t>		n3i32;
	typedef	n3<int64_t>		n3i64;

	typedef	const n3char	cn3char		;
	typedef	const n3uchar	cn3uchar	;
	typedef	const n3f32		cn3f32		;
	typedef	const n3f64		cn3f64		;
	typedef	const n3u8		cn3u8		;
	typedef	const n3u16		cn3u16		;
	typedef	const n3u32		cn3u32		;
	typedef	const n3u64		cn3u64		;
	typedef	const n3i8		cn3i8		;
	typedef	const n3i16		cn3i16		;
	typedef	const n3i32		cn3i32		;
	typedef	const n3i64		cn3i64		;

#pragma pack(pop)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
} // namespace

#define N3_F32 "{%f, %f, %f}"
#define N3_F64 "{%g, %g, %g}"
#define N3_I32 "{%i, %i, %i}"
#define N3_U32 "{%u, %u, %u}"
#define N3_I64 "{%lli, %lli, %lli}"
#define N3_U64 "{%llu, %llu, %llu}"

#define gpk_xyz(n3var) n3var.x, n3var.y, n3var.z

#endif // GPK_N3_H_23627
