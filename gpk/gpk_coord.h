#include "gpk_math.h"
#include "gpk_bit.h"
#include "gpk_eval.h"
#include "gpk_log.h"

#ifndef GPK_COORD_H_928374982364923322
#define GPK_COORD_H_928374982364923322

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tBase>
	struct SCoord2 {
		typedef									SCoord2<_tBase>			TCoord2;
												_tBase					x, y;
		//
		constexpr								TCoord2					operator+				(const TCoord2& other)												const	noexcept	{ return {x + other.x, y + other.y};									}
		constexpr								TCoord2					operator-				(const TCoord2& other)												const	noexcept	{ return {x - other.x, y - other.y};									}
		constexpr								TCoord2					operator*				(double			scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		constexpr								TCoord2					operator/				(double			scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		constexpr								TCoord2					operator*				(int64_t		scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		constexpr								TCoord2					operator/				(int64_t		scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		constexpr								TCoord2					operator*				(uint64_t		scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		constexpr								TCoord2					operator/				(uint64_t		scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		constexpr								TCoord2					operator*				(int32_t		scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		constexpr								TCoord2					operator/				(int32_t		scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		constexpr								TCoord2					operator*				(uint32_t		scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};					}
		constexpr								TCoord2					operator/				(uint32_t		scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};					}
		//
												TCoord2&				operator+=				(const TCoord2& other)														noexcept	{ x += other.x; y += other.y;							return *this;	}
												TCoord2&				operator-=				(const TCoord2& other)														noexcept	{ x -= other.x; y -= other.y;							return *this;	}
												TCoord2&				operator*=				(double			scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
												TCoord2&				operator/=				(double			scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
												TCoord2&				operator*=				(int64_t		scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
												TCoord2&				operator/=				(int64_t		scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
												TCoord2&				operator*=				(int32_t		scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
												TCoord2&				operator/=				(int32_t		scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
												TCoord2&				operator*=				(uint32_t		scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
												TCoord2&				operator/=				(uint32_t		scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
												TCoord2&				operator*=				(uint64_t		scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;	}
												TCoord2&				operator/=				(uint64_t		scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;	}
		//
		constexpr								bool					operator==				(const TCoord2& other)												const	noexcept	{ return x == other.x && y == other.y;									}
		inline constexpr						bool					operator!=				(const TCoord2& other)												const	noexcept	{ return !operator==(other);											}
		constexpr								TCoord2					operator-				()																	const	noexcept	{ return {x*-1, y*-1};													}
		//
		template<typename _t>
		constexpr inline						SCoord2<_t>				Cast					()																	const	noexcept	{ return {(_t)x, (_t)y};																																				}
		inline									TCoord2&				InPlaceScale			(double			scalar)														noexcept	{ return *this *= scalar;																																				}
		inline									TCoord2&				InPlaceScale			(double scalarx, double scalary)											noexcept	{ return *this = {(_tBase)(x * scalarx), (_tBase)(y * scalary)};																										}
		inline									TCoord2&				InPlaceNormalize		()																						{ const _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::gpk::sqrt_safe	(sqLen) : *this;																	}
		inline constexpr						TCoord2					GetScaled				(double			scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};																													}
		inline constexpr						TCoord2					GetScaled				(double scalarx, double scalary)									const	noexcept	{ return {(_tBase)(x * scalarx), (_tBase)(y * scalary)};																												}
		inline									TCoord2					GetNormalized			()																	const				{ const _tBase sqLen = LengthSquared(); if(sqLen) { const double len = ::gpk::sqrt_safe	(sqLen); return {(_tBase)(x / len), (_tBase)(y / len)}; } else return {x, y};	} 
		constexpr								double					Dot						(const TCoord2& other)												const	noexcept	{ return x * other.x + y * other.y;																																		}
		constexpr								_tBase					LengthSquared			()																	const	noexcept	{ return x * x + y * y;																																					}
		constexpr								double					Length					()																	const				{ const _tBase sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0;																								}
		constexpr								double					AngleWith				(const TCoord2& other)												const				{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0;													}
												void					AddScaled				(const TCoord2& vectorToScaleAndAdd, float scale)										{
			x																+= vectorToScaleAndAdd.x * scale;
			y																+= vectorToScaleAndAdd.y * scale;
		}
												TCoord2&				Rotate					(double theta)																			{
			const ::gpk::SPairSinCos											pairSinCos				= ::gpk::getSinCos(theta);
			const double														px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 
			y																= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x																= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2

	template<typename _tBase>
	struct SCoord3 {
		typedef									SCoord3<_tBase>			TCoord3;
												_tBase					x, y, z;
		inline									const _tBase&			operator[]				(uint32_t index)											const				{ throw_if(index > 2, "", "Invalid quaternion element being accessed: %u", index); return ((&x)[index]); }
		inline									_tBase&					operator[]				(uint32_t index)																{ throw_if(index > 2, "", "Invalid quaternion element being accessed: %u", index); return ((&x)[index]); }
		//
		constexpr								TCoord3					operator+				(const TCoord3& other)												const	noexcept	{ return {x + other.x, y + other.y, z + other.z};												}
		constexpr								TCoord3					operator-				(const TCoord3& other)												const	noexcept	{ return {x - other.x, y - other.y, z - other.z};												}
		constexpr								TCoord3					operator*				(double scalar)														const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr								TCoord3					operator/				(double scalar)														const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr								TCoord3					operator*				(int64_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr								TCoord3					operator/				(int64_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr								TCoord3					operator*				(uint64_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr								TCoord3					operator/				(uint64_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr								TCoord3					operator*				(int32_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr								TCoord3					operator/				(int32_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr								TCoord3					operator*				(uint32_t scalar)													const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr								TCoord3					operator/				(uint32_t scalar)													const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		//
												TCoord3&				operator+=				(const TCoord3& other)														noexcept	{ x += other.x; y += other.y; z += other.z;										return *this;	}
												TCoord3&				operator-=				(const TCoord3& other)														noexcept	{ x -= other.x; y -= other.y; z -= other.z;										return *this;	}
												TCoord3&				operator*=				(double scalar)																noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar);	return *this;	}
												TCoord3&				operator/=				(double scalar)																			{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar);	return *this;	}
												TCoord3&				operator*=				(int64_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar);	return *this;	}
												TCoord3&				operator/=				(int64_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar);	return *this;	}
												TCoord3&				operator*=				(int32_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar);	return *this;	}
												TCoord3&				operator/=				(int32_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar);	return *this;	}
												TCoord3&				operator*=				(uint32_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar);	return *this;	}
												TCoord3&				operator/=				(uint32_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar);	return *this;	}
												TCoord3&				operator*=				(uint64_t scalar)															noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar);	return *this;	}
												TCoord3&				operator/=				(uint64_t scalar)																		{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar);	return *this;	}
		//
		constexpr								bool					operator==				(const TCoord3& other)												const	noexcept	{ return x == other.x && y == other.y && z == other.z;											}
		inline constexpr						bool					operator!=				(const TCoord3& other)												const	noexcept	{ return !operator==(other);																	}
		constexpr								TCoord3					operator-				()																	const	noexcept	{ return {x*-1, y*-1, z*-1};																	}
		//
		template<typename _t>
		inline constexpr 						SCoord3<_t>				Cast					()																	const	noexcept	{ return {(_t)x, (_t)y, (_t)z};																							}
		inline									TCoord3&				Scale					(double scalar)																noexcept	{ return *this *= scalar;																								}
		inline									TCoord3&				Scale					(const TCoord3& other)														noexcept	{ x *= other.x; y *= other.y; z *= other.z; return *this;																}
		inline									TCoord3&				Normalize				()																						{ const _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::gpk::sqrt_safe(sqLen) : *this;						}
		constexpr								double					Dot						(const TCoord3& other)												const	noexcept	{ return x * other.x + y * other.y + z * other.z;																		}
		constexpr								_tBase					LengthSquared			()																	const	noexcept	{ return x * x + y * y + z * z;																							}
		constexpr								double					Length					()																	const				{ const _tBase sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0;												}
		constexpr								double					AngleWith				(const TCoord3& other)												const				{ double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0;	}
												void					AddScaled				(const TCoord3& vectorToScaleAndAdd, float scale)										{
			x																+= vectorToScaleAndAdd.x * scale;
			y																+= vectorToScaleAndAdd.y * scale;
			z																+= vectorToScaleAndAdd.z * scale;
		}

		constexpr								TCoord3					Cross					(const TCoord3& right)												const	noexcept	{ return {y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x };	}
												TCoord3&				Cross					(const TCoord3& vector1, const TCoord3& vector2)							noexcept	{ 
			x																= vector1.y * vector2.z - vector1.z * vector2.y;
			y																= vector1.z * vector2.x - vector1.x * vector2.z;
			z																= vector1.x * vector2.y - vector1.y * vector2.x;
			return *this;
		}
												TCoord3&				CrossAndNormalize		(const TCoord3& in_vLeft, const TCoord3& in_vRight)										{ 
			x																= in_vLeft->y * in_vRight->z - in_vLeft->z * in_vRight->y;
			y																= in_vLeft->z * in_vRight->x - in_vLeft->x * in_vRight->z;
			z																= in_vLeft->x * in_vRight->y - in_vLeft->y * in_vRight->x;
			Normalize(); 
			return *this;
		};
												TCoord3&				RotateX					(double theta)																			{
			const ::gpk::SPairSinCos											pairSinCos				= ::gpk::getSinCos(theta);
			const double														pz						= y * pairSinCos.Cos - z * pairSinCos.Sin; 
			y																= (_tBase)(y * pairSinCos.Sin + z * pairSinCos.Cos);
			z																= (_tBase)pz;
			return *this;
		}

												TCoord3&				RotateY					(double theta)																			{
			const ::gpk::SPairSinCos											pairSinCos				= ::gpk::getSinCos(theta);
			const double														px						= x * pairSinCos.Cos - z * pairSinCos.Sin; 
			z																= (_tBase)(x * pairSinCos.Sin + z * pairSinCos.Cos);
			x																= (_tBase)px;	
			return *this;
		}

												TCoord3&				RotateZ					(double theta)																			{
			const ::gpk::SPairSinCos											pairSinCos				= ::gpk::getSinCos(theta);
			const double														px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 
			y																= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x																= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2

	template<typename _tBase>
	struct SQuaternion {
		typedef				SQuaternion	<_tBase>	TQuat;
		typedef				SCoord3		<_tBase>	TCoord3;
							_tBase					x, y, z, w;
		//
		inline				const _tBase&	operator[]				(uint32_t index)											const				{ throw_if(index > 3, "Invalid quaternion element being accessed: %u", index); return *((&x)[index]); }
		inline				_tBase&			operator[]				(uint32_t index)																{ throw_if(index > 3, "Invalid quaternion element being accessed: %u", index); return *((&x)[index]); }
		// 
		constexpr			bool			operator ==				(const TQuat& other)										const	noexcept	{ return x == other.x && y == other.y && z == other.z && w == other.w; }
		constexpr inline	bool			operator !=				(const TQuat& other)										const	noexcept	{ return !operator==(other); }
		//
		constexpr			TQuat			operator +				(const TQuat& other)										const	noexcept	{ return { x + other.x, y + other.y, z + other.z, w + other.w }; }
		constexpr			TQuat			operator -				(const TQuat& other)										const	noexcept	{ return { x - other.x, y - other.y, z - other.z, w - other.w }; }
		constexpr			TQuat			operator *				(double scalar)												const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TQuat			operator /				(double scalar)												const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }

							TQuat			operator *				(const TQuat& q)											const	noexcept	{
								TQuat			r;
								r.x			= w*q.x + x*q.w + y*q.z - z*q.y;
								r.y			= w*q.y + y*q.w + z*q.x - x*q.z;
								r.z			= w*q.z + z*q.w + x*q.y - y*q.x;
								r.w			= w*q.w - x*q.x - y*q.y - z*q.z;
		
								return r;
							}
							TQuat			operator *				(const TCoord3& v)											const	noexcept	{
								return 
									{	  w*v.x + y*v.z - z*v.y
									,	  w*v.y + z*v.x - x*v.z
									,	  w*v.z + x*v.y - y*v.x
									,	-(x*v.x + y*v.y + z*v.z) 
									};
							}
							TQuat&			operator+=				(const TQuat& other)												noexcept	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this;													}
							TQuat&			operator-=				(const TQuat& other)												noexcept	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;													}
							TQuat&			operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this;	}
							TQuat&			operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this;	}
		inline				TQuat&			operator*=				(const TQuat& q)													noexcept	{ return *this = operator*(q); }
		// Unary operators
		constexpr inline	TQuat			operator-				()															const	noexcept	{ return {x*-1, y*-1, z*-1, w*-1};																						}
		constexpr inline	TQuat			operator~				()															const	noexcept	{ return {-x, -y, -z, w};																								}
		template<typename _t>
		constexpr inline	SQuaternion<_t>	Cast					()															const	noexcept	{ return {(_t)x, (_t)y, (_t)z, (_t)w};																					}
		inline				void			Identity				()																	noexcept	{ x = y = z = 0.0f; w = 1.0f;																							}
		inline				TQuat&			Normalize				()																	noexcept	{ _tBase sqLen = LengthSquared(); if(sqLen ) return *this /= ::gpk::sqrt_safe(sqLen); Identity(); return *this;		}
		constexpr			double			Dot						(const TQuat& other)										const	noexcept	{ return x*other.x + y*other.y + z*other.z + w*other.w;																	}
		constexpr inline	_tBase			LengthSquared			()															const	noexcept	{ return x * x + y * y + z * z + w * w;																					}
		inline constexpr	double			Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;												}
		inline				TQuat&			LinearInterpolate		(const TQuat &p, const TQuat &q, double fTime)						noexcept	{ return *this = ((q-p)*fTime)+p;																						}
							//void			AddScaled				(const TCoord4& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
							void			AddScaled				(const TCoord3& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
		inline				void			SetRotation				(const TQuat& q1, const TQuat& q2)									noexcept	{ return *this = q1 * q2 * ~q1;																							}
							TCoord3			RotateVector			(const TCoord3 &v)											const	noexcept	{ 
			const TQuat								t						= {x, y, z, w};
			const TQuat								r						= t * v * (~t);
			return {r.x, r.y, r.z}; 
		}
							TQuat&			SLERP					(const TQuat& p, const TQuat& q, double fTime)									{
			//Calculate the dot product		
			double									fDot					= Dot(q);
		
			TQuat									q_;
			if(fDot < 0.0f) { 
				q_									= -q; 
				fDot								= -fDot; 
			} 
 		
			if(fDot < 1.00001f && fDot > 0.99999f) 
				return ::gpk::interpolate_linear(p, q_, fTime); 
 		
			//calculate the angle between the quaternions  
			const	double							fTheta					= ::acos(fDot); 
			return *this						= (p * ::sin(fTheta * (1 - fTime)) + q_ * ::sin(fTheta * fTime)) / ::sin(fTheta); 
		}
		// Convert from Euler Angles	
		inline				TQuat&			MakeFromEulerTaitBryan	(const TCoord3& v)																{ return MakeFromEulerTaitBryan(v.x, v.y, v.z);																			}
							TQuat&			MakeFromEulerTaitBryan	(double fPitch, double fYaw, double fRoll)										{
			 //Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll and multiply those together. the calculation below does the same, just shorter.
			fPitch								*= 0.5f;//
			fYaw								*= 0.5f;//
			fRoll								*= 0.5f;//

			const SPairSinCos						pairSinCosX				= ::gpk::getSinCos(fPitch	);
			const SPairSinCos						pairSinCosY				= ::gpk::getSinCos(fYaw	);
			const SPairSinCos						pairSinCosZ				= ::gpk::getSinCos(fRoll	);
		
			double									cYcZ					= pairSinCosY.Cos * pairSinCosZ.Cos;
			double									sYsZ					= pairSinCosY.Sin * pairSinCosZ.Sin;
			double									cYsZ					= pairSinCosY.Cos * pairSinCosZ.Sin;
			double									sYcZ					= pairSinCosY.Sin * pairSinCosZ.Cos;
		
			w									= (_tBase)(pairSinCosX.Cos * cYcZ + pairSinCosX.Sin * sYsZ);
			x									= (_tBase)(pairSinCosX.Sin * cYcZ - pairSinCosX.Cos * sYsZ);
			y									= (_tBase)(pairSinCosX.Cos * sYcZ + pairSinCosX.Sin * cYsZ);
			z									= (_tBase)(pairSinCosX.Cos * cYsZ - pairSinCosX.Sin * sYcZ);
		
			return Normalize();
		}
							void			GetEulersTaitBryan		(double* fPitch, double* fYaw, double* fRoll)									{
			double									r11, r21, r31, r32, r33, r12, r13;
			double									q00, q11, q22, q33;
			double									tmp;
		
			q00									= w * w;
			q11									= x * x;
			q22									= y * y;
			q33									= z * z;
		
			r11									= q00 + q11 - q22 - q33;
			r21									= 2 * (x*y + w*z);
			r31									= 2 * (x*z - w*y);
			r32									= 2 * (y*z + w*x);
			r33									= q00 - q11 - q22 + q33;
		
#if defined( __ANDROID__ ) || defined( __LINUX__ )
			tmp									= ::std::abs(r31);
#else
			tmp									= ::abs(r31);
#endif
			if(tmp > 0.999999) {
				r12									= 2 * (x*y - w*z);
				r13									= 2 * (x*z + w*y);
				*fPitch								= 0.0f;
				*fYaw								= -((::gpk::math_pi2) * r31/tmp);
				*fRoll								= ::atan2(-r12, -r31*r13);
			}
			else {
				*fPitch								 = ::atan2(r32, r33);
				*fYaw								 = ::asin(-r31);
				*fRoll								 = ::atan2(r21, r11);
			}
		} //
	}; // struct SQuaternion


#define GPK_DEFAULT_OPERATOR_NE(_otherType, ...)	\
		inline constexpr									bool						operator!=				(const _otherType		& other)											const	noexcept	{ return !operator==(other);	}	\
		inline constexpr									bool						operator==				(const _otherType		& other)											const	noexcept	{ return __VA_ARGS__;			}

	// ---- Geometric figures and other coord-related POD structs.
	template<typename _tElement>	struct SRectLimits		{ _tElement									Left, Top, Right, Bottom; GPK_DEFAULT_OPERATOR_NE(SRectLimits	<_tElement>, Left	== other.Left	&& Top		== other.Top	&& Right == other.Right && Bottom == other.Bottom); };
	template<typename _tElement>	struct SRange			{ _tElement									Offset, Count			; GPK_DEFAULT_OPERATOR_NE(SRange		<_tElement>, Offset	== other.Offset	&& Count	== other.Count					); };
	template<typename _tElement>	struct SLine2D			{ ::gpk::SCoord2<_tElement>					A, B					; GPK_DEFAULT_OPERATOR_NE(SLine2D		<_tElement>, A		== other.A		&& B		== other.B						); };
	template<typename _tElement>	struct STriangle2D		{ ::gpk::SCoord2<_tElement>					A, B, C					; GPK_DEFAULT_OPERATOR_NE(STriangle2D	<_tElement>, A		== other.A		&& B		== other.B		&& C == other.C	); };
	template<typename _tElement>	struct SCircle2D		{ double Radius; ::gpk::SCoord2<_tElement>	Center					; GPK_DEFAULT_OPERATOR_NE(SCircle2D		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };
	template<typename _tElement>	struct SSphere2D		{ double Radius; ::gpk::SCoord2<_tElement>	Center					; GPK_DEFAULT_OPERATOR_NE(SSphere2D		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };

	template<typename _tElement>	struct SLine3D			{ ::gpk::SCoord3<_tElement>					A, B					; GPK_DEFAULT_OPERATOR_NE(SLine3D		<_tElement>, A		== other.A		&& B		== other.B						); };
	template<typename _tElement>	struct SRectangle3D		{ ::gpk::SCoord3<_tElement>					Offset, Size			; GPK_DEFAULT_OPERATOR_NE(SRectangle3D	<_tElement>, Offset	== other.Offset	&& Size		== other.Size					); };
	template<typename _tElement>	struct SCircle3D		{ double Radius; ::gpk::SCoord3<_tElement>	Center					; GPK_DEFAULT_OPERATOR_NE(SCircle3D		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };
	template<typename _tElement>	struct SSphere3D		{ double Radius; ::gpk::SCoord3<_tElement>	Center					; GPK_DEFAULT_OPERATOR_NE(SSphere3D		<_tElement>, Center	== other.Center	&& Radius	== other.Radius					); };
	template<typename _tElement>	struct STriangle3D		{ ::gpk::SCoord3<_tElement>					A, B, C					; GPK_DEFAULT_OPERATOR_NE(STriangle3D	<_tElement>, A		== other.A		&& B		== other.B		&& C == other.C	); };

	template<typename _tElement>	struct STriangleWeights	{ _tElement									A, B, C					; GPK_DEFAULT_OPERATOR_NE(STriangle3D	<_tElement>, A		== other.A		&& B		== other.B		&& C == other.C	); };

	template<typename _tElement>	struct SRectangle2D		{ 
							::gpk::SCoord2<_tElement>					Offset, Size; 

				GPK_DEFAULT_OPERATOR_NE(SRectangle2D<_tElement>, Offset	== other.Offset	&& Size == other.Size); 

		inline				::gpk::SCoord2<_tElement>					Limit									()																			const	noexcept	{ return Offset + Size; } 
	};

	template<typename _tElement>
							STriangle2D<_tElement>&						translate								(::gpk::STriangle2D<_tElement>& triangle, const ::gpk::SCoord2<_tElement>& translation)									{
		triangle.A															+= translation;
		triangle.B															+= translation;
		triangle.C															+= translation;
		return triangle;
	}

	template<typename _tElement>
							STriangle3D<_tElement>&						translate								(::gpk::STriangle3D<_tElement>& triangle, const ::gpk::SCoord3<_tElement>& translation)									{
		triangle.A															+= translation;
		triangle.B															+= translation;
		triangle.C															+= translation;
		return triangle;
	}
	template<typename _tElement>
							STriangle3D<_tElement>&						scale								(::gpk::STriangle3D<_tElement>& triangle, const ::gpk::SCoord3<_tElement>& scale)									{
		triangle.A.Scale(scale);
		triangle.B.Scale(scale);
		triangle.C.Scale(scale);
		return triangle;
	}

#pragma pack(pop)

	// ---- Line
	template<typename _tElement>	static inline constexpr	_tElement				rise					(const SLine2D<_tElement>& line)													noexcept	{ return line.B.y - line.A.y;		}
	template<typename _tElement>	static inline constexpr	_tElement				run						(const SLine2D<_tElement>& line)													noexcept	{ return line.B.x - line.A.x;		}
	template<typename _tElement>	static inline constexpr	_tElement				slope					(const SLine2D<_tElement>& line)																{ return rise(line) / run(line);	}
	template<typename _tElement>	static		  constexpr	_tElement				orient2d				(const ::gpk::SLine2D<_tElement>& segment, const ::gpk::SCoord2<_tElement>& point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
	template<typename _tElement>	static		  constexpr	_tElement				orient2d3d				(const ::gpk::SLine3D<_tElement>& segment, const ::gpk::SCoord2<_tElement>& point)	noexcept	{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

	// ---- Collision
	template<typename _tElement>	static					bool					raySegmentIntersect		(SCoord2<_tElement> r_d, SCoord2<_tElement> r_p, SCoord2<_tElement> s_d, SCoord2<_tElement> s_p)								{
		const double																		t2						= (r_d.x * (s_p.y - r_p.y) + r_d.y * (r_p.x - s_p.x)) / (s_d.x * r_d.y - s_d.y * r_d.x);
		const double																		t1						= (s_p.x + s_d.x * t2 - r_p.x) / r_d.x;	// Plug the value of T2 to get T1
		return (t2 > 0 && 0 < t2 && t2 < 1);
	}

	template<typename _tElement>	static					bool					sphereOverlaps			(const SSphere2D<_tElement> &sphereA, const SSphere2D<_tElement> &sphereB)	noexcept	{
		const double																		distanceSquared			= (sphereA.Center - sphereB.Center).LengthSquared();
		const double																		radiiSum				= sphereA.Radius + sphereB.Radius;
		return distanceSquared < (radiiSum * radiiSum);	// check squared distance against squared radius
	}

	// Returns the volume of a sphere. This is used to calculate how to recurse into the bounding volume tree. For a bounding sphere it is a simple calculation.
	template<typename _tElement>	static					double					sphereSize				(const SSphere2D<_tElement> &sphere)										noexcept	{ return 1.3333333333333333 * ::gpk::math_pi * sphere.Radius * sphere.Radius * sphere.Radius; }


#pragma pack(push)
	enum AXIS : uint8_t
		{	AXIS_XPOSITIVE			= 0x1
		,	AXIS_XNEGATIVE			= 0x2
		,	AXIS_YPOSITIVE			= 0x4
		,	AXIS_YNEGATIVE			= 0x8
		,	AXIS_ZPOSITIVE			= 0x10
		,	AXIS_ZNEGATIVE			= 0x20
		};

	enum ALIGN : uint8_t
		{	ALIGN_RIGHT				= ((uint8_t)AXIS_XPOSITIVE)
		,	ALIGN_LEFT				= ((uint8_t)AXIS_XNEGATIVE)
		,	ALIGN_BOTTOM			= ((uint8_t)AXIS_YPOSITIVE)
		,	ALIGN_TOP				= ((uint8_t)AXIS_YNEGATIVE)
		,	ALIGN_FAR				= ((uint8_t)AXIS_ZPOSITIVE)
		,	ALIGN_NEAR				= ((uint8_t)AXIS_ZNEGATIVE)
		,	ALIGN_HCENTER			= ALIGN_LEFT	| ALIGN_RIGHT
		,	ALIGN_VCENTER			= ALIGN_TOP		| ALIGN_BOTTOM
		,	ALIGN_ZCENTER			= ALIGN_FAR		| ALIGN_NEAR
		,	ALIGN_TOP_LEFT			= ALIGN_TOP		| ALIGN_LEFT  
		,	ALIGN_TOP_RIGHT			= ALIGN_TOP		| ALIGN_RIGHT 
		,	ALIGN_BOTTOM_LEFT		= ALIGN_BOTTOM	| ALIGN_LEFT  
		,	ALIGN_BOTTOM_RIGHT		= ALIGN_BOTTOM	| ALIGN_RIGHT 
		,	ALIGN_CENTER_TOP		= ALIGN_HCENTER	| ALIGN_TOP	
		,	ALIGN_CENTER_BOTTOM		= ALIGN_HCENTER	| ALIGN_BOTTOM	
		,	ALIGN_CENTER_LEFT		= ALIGN_VCENTER	| ALIGN_LEFT		
		,	ALIGN_CENTER_RIGHT		= ALIGN_VCENTER	| ALIGN_RIGHT	
		,	ALIGN_CENTER			= ALIGN_VCENTER	| ALIGN_HCENTER
		};
#pragma pack(pop)
	//------------------------------------------------------------------------------------------------------------
	template <typename _tCoord>
				::gpk::SRectangle2D<_tCoord>&									realignRectangle		
					(	const ::gpk::SCoord2<uint32_t>			& targetSize
					,	const ::gpk::SRectangle2D<_tCoord>		& rectangleToRealign
					,	::gpk::SRectangle2D<_tCoord>			& rectangleRealigned
					,	ALIGN									align
					)																																					noexcept	{
		rectangleRealigned															= rectangleToRealign;
			 if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleRealigned.Offset.x += (_tCoord)(targetSize.x >> 1)	- (rectangleRealigned.Size.x >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleRealigned.Offset.x =  (_tCoord) targetSize.x			- (rectangleRealigned.Size.x + rectangleRealigned.Offset.x); }
		else													{}
	
			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleRealigned.Offset.y += (_tCoord)(targetSize.y >> 1)	- (rectangleRealigned.Size.y >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleRealigned.Offset.y =  (_tCoord) targetSize.y			- (rectangleRealigned.Size.y + rectangleRealigned.Offset.y); }
		else													{}
		return rectangleRealigned;
	}

	template <typename _tCoord>
				::gpk::SRectangle2D<_tCoord>&									realignRectangle		
					(	const ::gpk::SCoord2<uint32_t>			& targetSize
					,	const ::gpk::SRectangle2D<double>		& rectangleToRealign
					,	::gpk::SRectangle2D<double>				& rectangleRealigned
					,	ALIGN									align
					)																																					noexcept	{
		rectangleRealigned															= rectangleToRealign;
			 if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleRealigned.Offset.x += (_tCoord)(targetSize.x >> 1)	- (rectangleRealigned.Size.x >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleRealigned.Offset.x =  (_tCoord) targetSize.x			- (rectangleRealigned.Size.x + rectangleRealigned.Offset.x); }
		else													{}
	
			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleRealigned.Offset.y += (_tCoord)(targetSize.y >> 1)	- (rectangleRealigned.Size.y >> 1); }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleRealigned.Offset.y =  (_tCoord) targetSize.y			- (rectangleRealigned.Size.y + rectangleRealigned.Offset.y); }
		else													{}
		return rectangleRealigned;
	}

	template <typename _tCoord>
				::gpk::SRectangle2D<_tCoord>&									dockRectangle		
					(	const ::gpk::SRectangle2D<_tCoord>		& rectangleToDockTo
					,	const ::gpk::SRectangle2D<_tCoord>		& rectangleToDock
					,	::gpk::SRectangle2D<_tCoord>			& rectangleDocked
					,	ALIGN									align
					)																																					noexcept	{
		rectangleDocked																= rectangleToDock;
			 /*if(::gpk::bit_true(align, ::gpk::ALIGN_HCENTER	))	{ rectangleDocked.Offset.x = (rectangleToDockTo.Size.x >> 1)	- (rectangleDocked.Size.x >> 1)	+ rectangleDocked.Offset.x; }
		else*/if(::gpk::bit_true(align, ::gpk::ALIGN_RIGHT	))	{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x  - (rectangleDocked.Size.x		+ rectangleDocked.Offset.x); }
		else													{ rectangleDocked.Offset.x = rectangleToDockTo.Size.x; }
	
			 if(::gpk::bit_true(align, ::gpk::ALIGN_VCENTER	))	{ rectangleDocked.Offset.y = (rectangleToDockTo.Size.y >> 1)	- (rectangleDocked.Size.y >> 1)	+ rectangleDocked.Offset.y; }
		else if(::gpk::bit_true(align, ::gpk::ALIGN_BOTTOM	))	{ rectangleDocked.Offset.y = rectangleToDockTo.Size.y			- (rectangleDocked.Size.y		+ rectangleDocked.Offset.y); }
		else													{}
		return rectangleDocked;
	}

	//------------------------------------------------------------------------------------------------------------
	template <typename _tCoord>
					::gpk::SCoord2<_tCoord>&										realignCoord		
					(	const ::gpk::SCoord2<uint32_t>		& targetSize
					,	const ::gpk::SCoord2<_tCoord>		& coordToRealign
					,	::gpk::SCoord2<_tCoord>				& coordRealigned
					,	::gpk::ALIGN						align
					)																																					noexcept	{
		coordRealigned																	= coordToRealign;
 			 if gbit_true(align, ALIGN_CENTER	)	coordRealigned.x						+= (targetSize.x >> 1);	
		else if gbit_true(align, ALIGN_RIGHT	)	coordRealigned.x						= targetSize.x - coordToRealign.x;			

			 if gbit_true(align, ALIGN_VCENTER	)	coordRealigned.y						+= (targetSize.y >> 1);	
		else if gbit_true(align, ALIGN_BOTTOM	)	coordRealigned.y						= targetSize.y - coordToRealign.y;			
		return coordRealigned;
	}

	template<typename _tValue>	
	static constexpr	bool														in_range		(const ::gpk::SCoord2<_tValue>& valueToTest, const ::gpk::SCoord2<_tValue>& rangeStart, const ::gpk::SCoord2<_tValue>& rangeStop)	noexcept	{ 
		return	::gpk::in_range(valueToTest.x, rangeStart.x, rangeStop.x) 
			&&	::gpk::in_range(valueToTest.y, rangeStart.y, rangeStop.y)
			;
	}

	template<typename _tValue>	
	static constexpr	bool														in_range		(const ::gpk::SCoord2<_tValue>& pointToTest, const ::gpk::SRectangle2D<_tValue>& area)	noexcept	{ 
		return	::gpk::in_range(pointToTest.x, area.Offset.x, area.Offset.x + area.Size.x) 
			&&	::gpk::in_range(pointToTest.y, area.Offset.y, area.Offset.y + area.Size.y)
			;
	}

	template<typename _tCoord>
	static inline		double														determinant		(const ::gpk::SLine2D<_tCoord>& line)													noexcept	{ return ::gpk::determinant((double)line.A.x, (double)line.A.y, (double)line.B.x, (double)line.B.y); }
}

#endif // GPK_COORD_H_928374982364923322