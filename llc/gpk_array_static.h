#include "gpk_cpow.h"
#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_23627
#define GPK_ARRAY_STATIC_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	tpl_t_nu2 stct array_static	{
		stxp		u2_c				N 	= 	_nu2;
		tdfTTCnst(_t);
		tydf		array_static<T, N>	TAStatic;
		tydf		view<TCnst>			VCnst;
		tydf		view<T>				V;
		T			Storage	[N]			;

		inln	oper		view<T>			()								{ rtrn Storage; }
		inxp	oper		view<TCnst>		()						csnx	{ rtrn Storage; }

		inln	TCnst&		oper[]			(u2_t index)			cnst	{ if_true_tef(index >= N, GPK_FMT_GE_U2, index, N); rtrn Storage[index]; }
		inln	T&			oper[]			(u2_t index)					{ if_true_tef(index >= N, GPK_FMT_GE_U2, index, N); rtrn Storage[index]; }
		GPK_DEFAULT_OPERATOR(TAStatic
			, (this->size() != other.size()) ? false
			: (this->begin() == other.begin()) ? true
			: ::gpk::equal(other.begin(), this->begin(), this->size())
			);
		inxp	u2_t		byte_count		()						csnx	{ rtrn N * szof(T);	}
		inxp	u2_t		bit_count		()						csnx	{ rtrn byte_count() * 8U;	}
		inxp	u2_t		size			()						csnx	{ rtrn N; }
		inln	T*			begin			()						nxpt	{ rtrn Storage;			}
		inln	T*			end				()						nxpt	{ rtrn Storage + N;	}
		inxp	TCnst*		begin			()						csnx	{ rtrn Storage;			}
		inxp	TCnst*		end				()						csnx	{ rtrn Storage + N;	}

		err_t				slice			(V & out, u2_t offset, u2_t count = (u2_t)-1)				{
			if_true_fef(offset > N, GPK_FMT_GT_U2, offset, N);
			u2_c					newSize			= N - offset;
			if(count != (u2_t)-1)
				if_true_fef(count > newSize, GPK_FMT_GT_U2, count, newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			rtrn out.size();
		}
		err_t				slice			(VCnst & out, u2_t offset, u2_t count = (u2_t)-1)	cnst	{
			if_true_fef(offset > N, GPK_FMT_GT_U2, offset, N);
			u2_c					newSize			= N - offset;
			if(count != (u2_t)-1)
				if_true_fef(count > newSize, GPK_FMT_GT_U2, count, newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			rtrn out.size();
		}
	};
#pragma pack(pop)

	tplTN2usng	asttc		= array_static<T, N>;
	tplTN2usng	astatic		= array_static<T, N>;

	tplN2uusng	astvcst_t	= astatic<vcst_t	, N>;	
	tplN2uusng	astvstr_t	= astatic<vstr_t	, N>;	

	tplN2uusng	astb8_t		= astatic<b8_t, N>; tplN2uusng	astb8_c	= const astb8_t<N>; 
	tplN2uusng	astuc_t		= astatic<uc_t, N>; tplN2uusng	astuc_c	= const astuc_t<N>; 
	tplN2uusng	astsc_t		= astatic<sc_t, N>; tplN2uusng	astsc_c	= const astsc_t<N>; 
	tplN2uusng	astu0_t		= astatic<u0_t, N>; tplN2uusng	astu0_c	= const astu0_t<N>; 
	tplN2uusng	astu1_t		= astatic<u1_t, N>; tplN2uusng	astu1_c	= const astu1_t<N>; 
	tplN2uusng	astu2_t		= astatic<u2_t, N>; tplN2uusng	astu2_c	= const astu2_t<N>; 
	tplN2uusng	astu3_t		= astatic<u3_t, N>; tplN2uusng	astu3_c	= const astu3_t<N>; 
	tplN2uusng	asts0_t		= astatic<s0_t, N>; tplN2uusng	asts0_c	= const asts0_t<N>; 
	tplN2uusng	asts1_t		= astatic<s1_t, N>; tplN2uusng	asts1_c	= const asts1_t<N>; 
	tplN2uusng	asts2_t		= astatic<s2_t, N>; tplN2uusng	asts2_c	= const asts2_t<N>; 
	tplN2uusng	asts3_t		= astatic<s3_t, N>; tplN2uusng	asts3_c	= const asts3_t<N>; 
	tplN2uusng	astf2_t		= astatic<f2_t, N>; tplN2uusng	astf2_c	= const astf2_t<N>; 
	tplN2uusng	astf3_t		= astatic<f3_t, N>; tplN2uusng	astf3_c	= const astf3_t<N>; 

	tplN2uusng	astvcu0_t 	= astatic<vcu0_t 	, N>; 
	tplN2uusng	astvcu1_t	= astatic<vcu1_t	, N>; 
	tplN2uusng	astvcu2_t	= astatic<vcu2_t	, N>; 
	tplN2uusng	astvcu3_t	= astatic<vcu3_t	, N>; 
	tplN2uusng	astvcs0_t 	= astatic<vcs0_t 	, N>; 
	tplN2uusng	astvcs1_t	= astatic<vcs1_t	, N>; 
	tplN2uusng	astvcs2_t	= astatic<vcs2_t	, N>; 
	tplN2uusng	astvcs3_t	= astatic<vcs3_t	, N>; 
	tplN2uusng	astvcf2_t	= astatic<vcf2_t	, N>; 
	tplN2uusng	astvcf3_t	= astatic<vcf3_t	, N>; 
	
	tplTN2sinx	u2_t	size		(asttc<T, N> & /*viewToTest*/)	nxpt	{ rtrn u2_t(N);				}
	tplTN2sinx	u2_t	byte_count	(asttc<T, N> & /*viewToTest*/)	nxpt	{ rtrn u2_t(sizeof(T) * N);	}
	tplTN2sinx	err_t	find		(cnst T & element, cnst astatic<T, N> & target, u2_t offset = 0)		{ rtrn find(element, view<cnst T>{target.begin(), target.size()}, offset); }
	tplN2ustin	vstr_t	str			(astsc_t<N> & arg)			{ rtrn arg.Storage; }
	tplN2usinx	vcst_t	str			(astsc_c<N> & arg)	nxpt	{ rtrn arg.Storage; }

	tplt<u2_t N>
	stct static_string :  astsc_t<N> {
		usng	astsc_t<N>	::Storage;
		u2_t					Count			= {};

		inxp					static_string	()						{}
		inxp					static_string	(astsc_c<N> & other)	: astsc_t<N>(other), Count(vcst_t(other.Storage).size()) {}

		inln	static_string&	oper=			(astsc_c<N> & other)	{ Count = vcst_t(other.Storage).size(); return astsc_t<N>::oper=(other); }

		inln	oper			vstr_t			()						{ rtrn Storage; }
		inxp	oper			vcst_t			()				csnx	{ rtrn Storage; }
		ndix	oper			sc_c* 			()				csnx	{ rtrn begin(); }

		inxp	u2_t			byte_count		()				csnx	{ rtrn vcst_t{Storage}.byte_count	(); } 
		inxp	u2_t			bit_count		()				csnx	{ rtrn vcst_t{Storage}.bit_count	(); } 
		inxp	u2_t			size			()				csnx	{ rtrn vcst_t{Storage}.size			(); }
		inln	sc_t*			begin			()				nxpt	{ rtrn vstr_t{Storage}.begin(); }
		inln	sc_t*			end				()				nxpt	{ rtrn vstr_t{Storage}.end  (); }
		inxp	sc_c*			begin			()				csnx	{ rtrn vcst_t{Storage}.begin(); }
		inxp	sc_c*			end				()				csnx	{ rtrn vcst_t{Storage}.end  (); }
	};
	tplN2uusng			sstring		= static_string<N>;
	tplTN2sinx	u2_t	size		(const sstring<N> & viewToTest)	nxpt	{ rtrn viewToTest.size(); }
	tplTN2sinx	u2_t	byte_count	(const sstring<N> & viewToTest)	nxpt	{ rtrn viewToTest.size(); }

	stin sstring<64>	str			(f2_t arg)	{ astsc_t< 64> dest = {}; sprintf_s(dest.Storage, "%f", arg); rtrn dest; }
	stin sstring<384>	str			(f3_t arg)	{ astsc_t<384> dest = {}; sprintf_s(dest.Storage, "%f", arg); rtrn dest; }
	stin sstring< 7>	str			(u1_t arg)	{ astsc_t<  7> dest = {}; sprintf_s(dest.Storage, "%u" , arg); rtrn dest; }
	stin sstring<12>	str			(u2_t arg)	{ astsc_t< 12> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U2, arg); rtrn dest; }
	stin sstring<22>	str			(u3_t arg)	{ astsc_t< 22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U3, arg); rtrn dest; }
	stin sstring< 8>	str			(s1_t arg)	{ astsc_t<  8> dest = {}; sprintf_s(dest.Storage, "%i", arg); rtrn dest; }
	stin sstring<13>	str			(s2_t arg)	{ astsc_t< 13> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_S2, arg); rtrn dest; }
	stin sstring<22>	str			(s3_t arg)	{ astsc_t< 22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_S3, arg); rtrn dest; }
	stxp sstring<5 >	str			(u0_t arg) {
		rtrn	(arg >= 100) ?	astsc_t<5>{digit<2>(arg), digit<1>(arg), digit<0>(arg), 0}
			:	(arg >=  10) ?	astsc_t<5>{digit<1>(arg), digit<0>(arg), 0}
			:					astsc_t<5>{digit<0>(arg), 0}
			;
	}
	stxp sstring<5 >	str			(s0_t arg) {
		rtrn	(arg >=    0) ?	str(u0_t(arg))
			:	(arg <= -100) ?	astsc_t<5>{'-', digit<2>((s1_t)-arg), digit<1>((s1_t)-arg), digit<0>((s1_t)-arg)}
			:	(arg <=  -10) ?	astsc_t<5>{'-', digit<1>((s1_t)-arg), digit<0>((s1_t)-arg)}
			:					astsc_t<5>{'-', digit<0>((s1_t)-arg)}
			;
	}
} // nmsp

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
