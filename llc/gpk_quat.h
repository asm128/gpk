#include "gpk_n3.h"

#ifndef GPK_QUAT_H_23627
#define GPK_QUAT_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct quat {
		_t	x, y, z, w;

		tydf	quat<_t>	TQuat;
		tydf	n3<_t>		Tn3;

		GPK_DEFAULT_OPERATOR(TQuat, x == other.x && y == other.y && z == other.z && w == other.w);

		inxp	TQuat	oper-	()						csnx	{ return {-x, -y, -z, -w}; }
		inxp	TQuat	oper~	()						csnx	{ return {-x, -y, -z,  w}; }
		cxpr	TQuat	oper+	(cnst TQuat & other)		csnx	{ return {x + other.x, y + other.y, z + other.z, w + other.w}; }
		cxpr	TQuat	oper-	(cnst TQuat & other)		csnx	{ return {x - other.x, y - other.y, z - other.z, w - other.w}; }
		cxpr	TQuat	oper*	(f3_t scalar)				csnx	{ return {_t(x * scalar), _t(y * scalar), _t(z * scalar), _t(w * scalar)}; }
				TQuat	oper/	(f3_t scalar)				cnst		{ return {_t(x / scalar), _t(y / scalar), _t(z / scalar), _t(w / scalar)}; }
				TQuat	oper*	(cnst TQuat & q)			csnx	{
			return
				{ _t(w * q.x + x * q.w + y * q.z - z * q.y)
				, _t(w * q.y + y * q.w + z * q.x - x * q.z)
				, _t(w * q.z + z * q.w + x * q.y - y * q.x)
				, _t(w * q.w - x * q.x - y * q.y - z * q.z)
				};
		}
		TQuat			oper*	(cnst Tn3 & v)				csnx	{
			return
				{ _t(w * v.x + y * v.z - z * v.y)
				, _t(w * v.y + z * v.x - x * v.z)
				, _t(w * v.z + x * v.y - y * v.x)
				, _t(-(x * v.x + y * v.y + z * v.z))
				};
		}
		TQuat &			oper*=	(cnst TQuat & other)		nxpt	{ return *this = *this * other; }
		TQuat &			oper+=	(cnst TQuat & other)		nxpt	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
		TQuat &			oper-=	(cnst TQuat & other)		nxpt	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
		TQuat &			oper*=	(f3_t scalar)				nxpt	{ x = _t(x * scalar); y = _t(y * scalar); z = _t(z * scalar); w = _t(w * scalar); return *this; }
		TQuat &			oper/=	(f3_t scalar)						{ x = _t(x / scalar); y = _t(y / scalar); z = _t(z / scalar); w = _t(w / scalar); return *this; }

		tpl_t2
		inxp	quat<_t2>	Cast	()	csnx	{ return {(_t2)x, (_t2)y, (_t2)z, (_t2)w}; }

		cxpr	_t			LengthSquared		()							csnx	{ return x * x + y * y + z * z + w * w; }
		inxp	f3_t		Length				()							cnst		{ cnst _t square = LengthSquared(); return square ? ::sqrt(square) : 0; }
		cxpr	f3_t		Dot					(cnst TQuat & other)		csnx	{ return x * other.x + y * other.y + z * other.z + w * other.w; }
		inline	TQuat &		Identity			()							nxpt	{ return *this = {0, 0, 0, 1}; }
		inline	TQuat &		Normalize			()							nxpt	{ cnst _t square = LengthSquared(); return square ? *this /= ::sqrt(square) : *this; }
		inline	TQuat		Normalized			()							csnx	{ cnst _t square = LengthSquared(); return square ? *this / ::sqrt(square) : *this; }
		inline	TQuat &		LinearInterpolate	(cnst TQuat & p, cnst TQuat & q, f3_t time)	nxpt	{ return *this = (q - p) * time + p; }
		inline	TQuat &		SetRotation			(cnst TQuat & q, cnst TQuat & p)			nxpt	{ return *this = q * p * ~q; }
		inline	TQuat &		MakeFromEuler		(cnst Tn3 & value)								{ return MakeFromEuler(value.x, value.y, value.z); }
		TQuat &				AddScaled			(cnst Tn3 & vector, f3_t scale)			nxpt	{
			TQuat scaled = {_t(vector.x * scale), _t(vector.y * scale), _t(vector.z * scale), _t{}};
			scaled *= *this;
			w += _t(scaled.w * .5);
			x += _t(scaled.x * .5);
			y += _t(scaled.y * .5);
			z += _t(scaled.z * .5);
			return *this;
		}
		Tn3				RotateVector		(cnst Tn3 & value)	csnx	{
			cnst TQuat result = *this * value * ~*this;
			return {result.x, result.y, result.z};
		}
		TQuat &			CreateFromAxisAngle	(cnst n3f2_t & axis, f3_t angle) {
			cnst f3_t halfAngle = angle * .5;
			cnst f3_t sine = sin(halfAngle);
			x = _t(axis.x * sine);
			y = _t(axis.y * sine);
			z = _t(axis.z * sine);
			w = _t(cos(halfAngle));
			return *this;
		}
		TQuat &			LookAt
			( cnst n3f2_t & sourcePoint
			, cnst n3f2_t & destinationPoint
			, cnst n3f2_t & up = {0, 1, 0}
			, cnst n3f2_t & front = {1, 0, 0}
			) {
			n3f2_t forward = (destinationPoint - sourcePoint).Normalize();
			cnst f3_t dot = front.Dot(forward);
			if(abs(dot + 1.0) < 0.000001)
				return *this = TQuat{up.x, up.y, up.z, -_t(math_pi)}.Normalize();
			if(abs(dot - 1.0) < 0.000001)
				return Identity();

			n3f2_t axis = front.Cross(forward);
			axis.Normalize();
			return CreateFromAxisAngle(axis, acos(dot));
		}
		TQuat &			SLERP	(cnst TQuat & p, cnst TQuat & q, f3_t time) {
			f3_t dot = p.Dot(q);
			TQuat target = q;
			if(dot < 0) {
				target = -q;
				dot = -dot;
			}
			if(dot < 1.00001 && dot > 0.99999)
				return LinearInterpolate(p, target, time);

			cnst f3_t theta = acos(dot);
			return *this = (p * sin(theta * (1 - time)) + target * sin(theta * time)) / sin(theta);
		}
		TQuat &			MakeFromEuler	(f3_t pitch, f3_t yaw, f3_t roll) {
			cnst SSinCos xPair = getSinCos(pitch * .5);
			cnst SSinCos yPair = getSinCos(yaw   * .5);
			cnst SSinCos zPair = getSinCos(roll  * .5);
			cnst f3_t yCosZCos = yPair.Cos * zPair.Cos;
			cnst f3_t ySinZSin = yPair.Sin * zPair.Sin;
			cnst f3_t yCosZSin = yPair.Cos * zPair.Sin;
			cnst f3_t ySinZCos = yPair.Sin * zPair.Cos;

			w = _t(xPair.Cos * yCosZCos + xPair.Sin * ySinZSin);
			x = _t(xPair.Sin * yCosZCos - xPair.Cos * ySinZSin);
			y = _t(xPair.Cos * ySinZCos + xPair.Sin * yCosZSin);
			z = _t(xPair.Cos * yCosZSin - xPair.Sin * ySinZCos);
			return Normalize();
		}
		void			GetEulersTaitBryan	(f3_t & pitch, f3_t & yaw, f3_t & roll) csnx {
			cnst f3_t q00 = w * w;
			cnst f3_t q11 = x * x;
			cnst f3_t q22 = y * y;
			cnst f3_t q33 = z * z;
			cnst f3_t r11 = q00 + q11 - q22 - q33;
			cnst f3_t r21 = 2 * (x * y + w * z);
			cnst f3_t r31 = 2 * (x * z - w * y);
			cnst f3_t r32 = 2 * (y * z + w * x);
			cnst f3_t r33 = q00 - q11 - q22 + q33;
			cnst f3_t absoluteR31 = abs(r31);

			if(absoluteR31 > 0.999999) {
				cnst f3_t r12 = 2 * (x * y - w * z);
				cnst f3_t r13 = 2 * (x * z + w * y);
				pitch = {};
				yaw = -(math_pi_2 * r31 / absoluteR31);
				roll = atan2(-r12, -r31 * r13);
				return;
			}
			pitch = atan2(r32, r33);
			yaw = asin(-r31);
			roll = atan2(r21, r11);
		}
	};
#pragma pack(pop)

	tydf quat<uc_t>	quatuc_t;	tdcs quatuc_t	quatuc_c;
	tydf quat<sc_t>	quatsc_t;	tdcs quatsc_t	quatsc_c;
	tydf quat<u0_t>	quatu0_t;	tdcs quatu0_t	quatu0_c;
	tydf quat<u1_t>	quatu1_t;	tdcs quatu1_t	quatu1_c;
	tydf quat<u2_t>	quatu2_t;	tdcs quatu2_t	quatu2_c;
	tydf quat<u3_t>	quatu3_t;	tdcs quatu3_t	quatu3_c;
	tydf quat<s0_t>	quats0_t;	tdcs quats0_t	quats0_c;
	tydf quat<s1_t>	quats1_t;	tdcs quats1_t	quats1_c;
	tydf quat<s2_t>	quats2_t;	tdcs quats2_t	quats2_c;
	tydf quat<s3_t>	quats3_t;	tdcs quats3_t	quats3_c;
	tydf quat<f2_t>	quatf2_t;	tdcs quatf2_t	quatf2_c;
	tydf quat<f3_t>	quatf3_t;	tdcs quatf3_t	quatf3_c;
}

#define QUAT_F2	"{%f, %f, %f, %f}"
#define QUAT_F3	"{%g, %g, %g, %g}"
#define QUAT_S2	"{%i, %i, %i, %i}"
#define QUAT_U2	"{%u, %u, %u, %u}"
#define QUAT_S3	"{%lli, %lli, %lli, %lli}"
#define QUAT_U3	"{%llu, %llu, %llu, %llu}"

#define gpk_xyzw(quatvar) quatvar.x, quatvar.y, quatvar.z, quatvar.w

#endif // GPK_QUAT_H_23627
