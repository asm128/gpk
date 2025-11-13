#include "gpk_cpow.h"
#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_23627
#define GPK_ARRAY_STATIC_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t_nu2 stct array_static	{
		stxp		u2_c				N 	= 	_nu2;
		tdfTTCnst(_t);
		tydf		array_static<T, N>	TAStatic;
		tydf		view<TCnst>			VCnst;
		tydf		view<T>				V;
		T			Storage	[N]			;

		inln	oper		view<T>			()								{ rtrn {Storage, N}; }
		inxp	oper		view<TCnst>		()						csnx	{ rtrn {Storage, N}; }

		inln	TCnst&		oper[]			(u2_t index)			cnst	{ gthrow_if(index >= N, GPK_FMT_GE_U2, index, N); rtrn Storage[index]; }
		inln	T&			oper[]			(u2_t index)					{ gthrow_if(index >= N, GPK_FMT_GE_U2, index, N); rtrn Storage[index]; }
		GPK_DEFAULT_OPERATOR(TAStatic
			, (this->size() != other.size()) ? false
			: (this->begin() == other.begin()) ? true
			: ::gpk::equal(other.begin(), this->begin(), this->size())
			);
		inxp	u2_t		byte_count		()						csnx	{ rtrn N * szof(T);	}
		inxp	u2_t		bit_count		()						csnx	{ rtrn byte_count() * 8U;	}
		inln	view<u0_t>	u8				()						nxpt	{ rtrn view<T		>(Storage, byte_count()).u8		(); }	
		inxp	view<u0_c>	u8				()						csnx	{ rtrn view<T		>(Storage, byte_count()).u8		(); }	
		inxp	view<u0_c>	cu8				()						csnx	{ rtrn view<TCnst	>(Storage, byte_count()).cu8	(); }	
		inln	view<sc_t>	c				()						nxpt	{ rtrn view<T		>(Storage, byte_count()).c		(); }
		inxp	view<sc_c>	cc				()						csnx	{ rtrn view<TCnst	>(Storage, byte_count()).cc		(); }	
		inxp	u2_c&		size			()						csnx	{ rtrn N; }
		inln	T*			begin			()						nxpt	{ rtrn Storage;			}
		inln	T*			end				()						nxpt	{ rtrn Storage + N;	}
		inxp	TCnst*		begin			()						csnx	{ rtrn Storage;			}
		inxp	TCnst*		end				()						csnx	{ rtrn Storage + N;	}

		err_t				slice			(V & out, u2_t offset, u2_t count = (u2_t)-1)				{
			ree_if(offset > N, GPK_FMT_GT_U2, offset, N);
			u2_c					newSize			= N - offset;
			if(count != (u2_t)-1)
				ree_if(count > newSize, GPK_FMT_GT_U2, count, newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			rtrn out.size();
		}
		err_t				slice			(VCnst & out, u2_t offset, u2_t count = (u2_t)-1)	cnst	{
			ree_if(offset > N, GPK_FMT_GT_U2, offset, N);
			u2_c					newSize			= N - offset;
			if(count != (u2_t)-1)
				ree_if(count > newSize, GPK_FMT_GT_U2, count, newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			rtrn out.size();
		}
	};
#pragma pack(pop)
	tplTN2usng	asttc		= ::gpk::array_static<T, N>;
	tplTN2usng	astatic		= ::gpk::array_static<T, N>;

	tplN2uusng	astvcs		= ::gpk::astatic<::gpk::vcst_t	, N>;	tplN2uusng astaticvcs	= ::gpk::astvcs	<N>;
	tplN2uusng	astvs		= ::gpk::astatic<::gpk::vs		, N>;	tplN2uusng astaticvs	= ::gpk::astvs	<N>;

	tplN2uusng	astb8_t		= ::gpk::astatic<b8_t, N>; tplN2uusng	astb8_c	= const ::gpk::astb8_t<N>; 
	tplN2uusng	astuc_t		= ::gpk::astatic<uc_t, N>; tplN2uusng	astuc_c	= const ::gpk::astuc_t<N>; tplN2uusng astuc 	= ::gpk::astatic<uc_t, N>; tplN2uusng astaticuc 	= ::gpk::astuc 	<N>; tplN2uusng astuchar	= ::gpk::astatic<uc_t, N>;
	tplN2uusng	astsc_t		= ::gpk::astatic<sc_t, N>; tplN2uusng	astsc_c	= const ::gpk::astsc_t<N>; tplN2uusng astc		= ::gpk::astatic<sc_t, N>; tplN2uusng astaticc		= ::gpk::astc	<N>; tplN2uusng astchar		= ::gpk::astatic<sc_t, N>;
	tplN2uusng	astu0_t		= ::gpk::astatic<u0_t, N>; tplN2uusng	astu0_c	= const ::gpk::astu0_t<N>; tplN2uusng astu8 	= ::gpk::astatic<u0_t, N>; tplN2uusng astaticu8 	= ::gpk::astu8 	<N>;
	tplN2uusng	astu1_t		= ::gpk::astatic<u1_t, N>; tplN2uusng	astu1_c	= const ::gpk::astu1_t<N>; tplN2uusng astu16	= ::gpk::astatic<u1_t, N>; tplN2uusng astaticu16	= ::gpk::astu16	<N>;
	tplN2uusng	astu2_t		= ::gpk::astatic<u2_t, N>; tplN2uusng	astu2_c	= const ::gpk::astu2_t<N>; tplN2uusng astu32	= ::gpk::astatic<u2_t, N>; tplN2uusng astaticu32	= ::gpk::astu32	<N>;
	tplN2uusng	astu3_t		= ::gpk::astatic<u3_t, N>; tplN2uusng	astu3_c	= const ::gpk::astu3_t<N>; tplN2uusng astu64	= ::gpk::astatic<u3_t, N>; tplN2uusng astaticu64	= ::gpk::astu64	<N>;
	tplN2uusng	asts0_t		= ::gpk::astatic<s0_t, N>; tplN2uusng	asts0_c	= const ::gpk::asts0_t<N>; tplN2uusng asti8 	= ::gpk::astatic<s0_t, N>; tplN2uusng astatici8 	= ::gpk::asti8 	<N>;
	tplN2uusng	asts1_t		= ::gpk::astatic<s1_t, N>; tplN2uusng	asts1_c	= const ::gpk::asts1_t<N>; tplN2uusng asti16	= ::gpk::astatic<s1_t, N>; tplN2uusng astatici16	= ::gpk::asti16	<N>;
	tplN2uusng	asts2_t		= ::gpk::astatic<s2_t, N>; tplN2uusng	asts2_c	= const ::gpk::asts2_t<N>; tplN2uusng asti32	= ::gpk::astatic<s2_t, N>; tplN2uusng astatici32	= ::gpk::asti32	<N>;
	tplN2uusng	asts3_t		= ::gpk::astatic<s3_t, N>; tplN2uusng	asts3_c	= const ::gpk::asts3_t<N>; tplN2uusng asti64	= ::gpk::astatic<s3_t, N>; tplN2uusng astatici64	= ::gpk::asti64	<N>;
	tplN2uusng	astf2_t		= ::gpk::astatic<f2_t, N>; tplN2uusng	astf2_c	= const ::gpk::astf2_t<N>; tplN2uusng astf32	= ::gpk::astatic<f2_t, N>; tplN2uusng astaticf32	= ::gpk::astf32	<N>;
	tplN2uusng	astf3_t		= ::gpk::astatic<f3_t, N>; tplN2uusng	astf3_c	= const ::gpk::astf3_t<N>; tplN2uusng astf64	= ::gpk::astatic<f3_t, N>; tplN2uusng astaticf64	= ::gpk::astf64	<N>;

	tplN2uusng	astvcuc 	= ::gpk::astatic<::gpk::vcuc 	, N>; tplN2uusng astaticvcuc 	= ::gpk::astvcuc <N>;
	tplN2uusng	astvcc		= ::gpk::astatic<::gpk::vcsc_t	, N>; tplN2uusng astaticvcc		= ::gpk::astvcc  <N>;
	tplN2uusng	astvcu8 	= ::gpk::astatic<::gpk::vcu0_t 	, N>; tplN2uusng astaticvcu8 	= ::gpk::astvcu8 <N>;
	tplN2uusng	astvcu16	= ::gpk::astatic<::gpk::vcu1_t	, N>; tplN2uusng astaticvcu16	= ::gpk::astvcu16<N>;
	tplN2uusng	astvcu32	= ::gpk::astatic<::gpk::vcu32	, N>; tplN2uusng astaticvcu32	= ::gpk::astvcu32<N>;
	tplN2uusng	astvcu64	= ::gpk::astatic<::gpk::vcu64	, N>; tplN2uusng astaticvcu64	= ::gpk::astvcu64<N>;
	tplN2uusng	astvci8 	= ::gpk::astatic<::gpk::vcs0_t 	, N>; tplN2uusng astaticvci8 	= ::gpk::astvci8 <N>;
	tplN2uusng	astvci16	= ::gpk::astatic<::gpk::vci16	, N>; tplN2uusng astaticvci16	= ::gpk::astvci16<N>;
	tplN2uusng	astvci32	= ::gpk::astatic<::gpk::vci32	, N>; tplN2uusng astaticvci32	= ::gpk::astvci32<N>;
	tplN2uusng	astvci64	= ::gpk::astatic<::gpk::vci64	, N>; tplN2uusng astaticvci64	= ::gpk::astvci64<N>;
	tplN2uusng	astvcf32	= ::gpk::astatic<::gpk::vcf32	, N>; tplN2uusng astaticvcf32	= ::gpk::astvcf32<N>;
	tplN2uusng	astvcf64	= ::gpk::astatic<::gpk::vcf64	, N>; tplN2uusng astaticvcf64	= ::gpk::astvcf64<N>;
	
	tplTN2sinx	u2_t	size		(::gpk::astatic<T, N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N);				}
	tplTN2sinx	u2_t	byte_count	(::gpk::astatic<T, N> viewToTest)		nxpt	{ rtrn u2_t(sizeof(T) * N);	}

	// Is this really worth it?
	tplN2usinx	u2_t	size(astc	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	//tplN2usinx	u2_t	size(astuc 	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astu8 	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astu16	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astu32	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astu64	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(asti8 	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(asti16	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(asti32	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(asti64	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astf32	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astf64	<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }

	tplN2usinx	u2_t	size(astvcc	 <N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	//tplN2usinx	u2_t	size(astvcuc <N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcu8 <N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcu16<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcu32<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcu64<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvci8 <N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvci16<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvci32<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvci64<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcf32<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }
	tplN2usinx	u2_t	size(astvcf64<N> /*viewToTest*/)	nxpt	{ rtrn u2_t(N); }

	tplTN2sinx	err_t	find		(cnst T & element, cnst astatic<cnst T, N> & target, u2_t offset = 0)	{ rtrn ::gpk::find(element, ::gpk::view<cnst T>{target}, offset); }
	stin astchar< 7>	str			(u1_t arg)	{ astchar< 7> dest = {}; sprintf_s(dest.Storage, "%u" , arg); rtrn dest; }
	stin astchar<12>	str			(u2_t arg)	{ astchar<12> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U2, arg); rtrn dest; }
	stin astchar<22>	str			(u3_t arg)	{ astchar<22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U3, arg); rtrn dest; }
	stin astchar< 7>	str			(s1_t arg)	{ astchar< 7> dest = {}; sprintf_s(dest.Storage, "%i", arg); rtrn dest; }
	stin astchar<12>	str			(s2_t arg)	{ astchar<12> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_S2, arg); rtrn dest; }
	stin astchar<22>	str			(s3_t arg)	{ astchar<22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U3, arg); rtrn dest; }
	stin astchar<12>	str			(f2_t arg)	{ astchar<12> dest = {}; sprintf_s(dest.Storage, "%f", arg); rtrn dest; }
	stin astchar<22>	str			(f3_t arg)	{ astchar<22> dest = {}; sprintf_s(dest.Storage, "%f", arg); rtrn dest; }
	stxp astchar<5 >	str			(u0_t arg) {
		rtrn	(arg >= 100) ?	astchar<5>{digit<2>(arg), digit<1>(arg), digit<0>(arg), 0}
			:	(arg >=  10) ?	astchar<5>{digit<1>(arg), digit<0>(arg), 0}
			:					astchar<5>{digit<0>(arg), 0}
			;
	}
	stxp astchar<5 >	str			(s0_t arg) {
		rtrn	(arg >=    0) ?	str(u0_t(arg))
			:	(arg <= -100) ?	astchar<5>{'-', digit<2>((int16_t)-arg), digit<1>((int16_t)-arg), digit<0>((int16_t)-arg)}
			:	(arg <=  -10) ?	astchar<5>{'-', digit<1>((int16_t)-arg), digit<0>((int16_t)-arg)}
			:					astchar<5>{'-', digit<0>((int16_t)-arg)}
			;
	}
} // namespace

#define GPK_USING_ASTATIC()					\
	usng	::gpk::astb8_t, ::gpk::astb8_c	\
		,	::gpk::astuc_t, ::gpk::astuc_c	\
		,	::gpk::astsc_t, ::gpk::astsc_c	\
		,	::gpk::astu0_t, ::gpk::astu0_c	\
		,	::gpk::astu1_t, ::gpk::astu1_c	\
		,	::gpk::astu2_t, ::gpk::astu2_c	\
		,	::gpk::astu3_t, ::gpk::astu3_c	\
		,	::gpk::asts0_t, ::gpk::asts0_c	\
		,	::gpk::asts1_t, ::gpk::asts1_c	\
		,	::gpk::asts2_t, ::gpk::asts2_c	\
		,	::gpk::asts3_t, ::gpk::asts3_c	\
		,	::gpk::astf2_t, ::gpk::astf2_c	\
		,	::gpk::astf3_t, ::gpk::astf3_c

#endif // GPK_ARRAY_STATIC_H_23627
