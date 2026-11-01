#include "gpk_keyval.h"
#include "gpk_functional.h"
#include "gpk_log.h"


#if defined(GPK_ATMEL)
#	ifdef max
#		undef max
#	endif
#	ifdef min
#		undef min
#	endif
#endif

#ifndef GPK_ARRAY_VIEW_H_23627
#define GPK_ARRAY_VIEW_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t clss view {
	prtc:
		// Properties / Member Variables
		_t						* Data			= 0;
		u2_t					Count			= 0;
	pblc:
		tdfTTCnst(_t);
		tydf	view<T>			TV;
		tydf	view<TCnst>		TConstView;
		tydf	view<TCnst>		TCstV;
		tdcs	view<T>			TVCst;
		tdcs	view<TCnst>		TCVCs;

		// Constructors
		inxp					view			()									nxpt	= dflt;
		inxp					view			(TVCst & other)						nxpt	= dflt;

		tplN2u	inxp			view			(T (&elements)[N])					nxpt	: Data(elements), Count(N)								{}
		tplN2u	inxp			view			(u2_t elementCount, T (&elements)[N])		: Data(elements), Count(::gpk::min(N, elementCount))	{}
		inln					view			(T * elements, u2_t elementCount)			: Data(elements), Count(elementCount)					{ if_true_tef(0 == elements && 0 != elementCount, "%" GPK_FMT_U2 " -> 0.", elementCount);	}

		// Operators
		inxp	oper			view<TCnst>		()									csnx	{ rtrn {Data, Count}; }
		T&						oper[]			(u2_t index)								{
			if_null_te(Data);
			if_true_tef(index >= Count, GPK_FMT_GE_U2, index, Count);
			rtrn Data[index];
		}
		cnst T&					oper[]			(u2_t index)						cnst	{
			if_null_te(Data);
			if_true_tef(index >= Count, GPK_FMT_GE_U2, index, Count);
			rtrn Data[index];
		}
		tplt<tpnm _tIndex> inln T&		oper[]			(_tIndex index)						{ rtrn operator[]((u2_t)index); }
		tplt<tpnm _tIndex> inln cnst T&	oper[]			(_tIndex index)				cnst	{ rtrn operator[]((u2_t)index); }
		ndxp	b8_t					oper!=			(TCVCs & other)				cnst	{ rtrn  !operator==(other); } // I had to add this for the android build not supporting C++20.
		ndxp	b8_t					oper==			(TCVCs & other)				cnst	{
			if(size() != other.size())
				rtrn false;
			if(begin() == other.begin())
				rtrn true;
			rtrn ::gpk::equal(other.begin(), begin(), size());
		}
		// Methods
		inxp	u2_t			byte_count		()														csnx	{ rtrn u2_t(Count * sizeof(T));	}
		inxp	u3_t			bit_count		()														csnx	{ rtrn byte_count() * 8ULL;			}
		inxp	u2_c&			size			()														csnx	{ rtrn Count;				}
		inxp	TCnst*			begin			()														csnx	{ rtrn Data;				}
		inxp	TCnst*			end				()														csnx	{ rtrn begin() + Count;	}
		inln	T*				begin			()														nxpt	{ rtrn Data;				}
		inln	T*				end				()														nxpt	{ rtrn begin() + Count;	}
		err_t					slice			(TV & out, u2_t offset, u2_t count = (u2_t)-1)					{
			if_true_fef(offset > Count, GPK_FMT_GT_U2, offset, (u2_t)Count);
			u2_c					newSize			= Count - offset;
			if(count != (u2_t)-1)
				if_true_fef(count > newSize, GPK_FMT_GT_U2, count, (u2_t)newSize);
			out						= {Data ? &Data[offset] : 0, ::gpk::min(newSize, count)};
			rtrn out.size();
		}
		err_t					slice			(TCstV & out, u2_t offset, u2_t count = (u2_t)-1)		cnst	{
			if_true_fef(offset > Count, GPK_FMT_GT_U2, offset, (u2_t)Count);
			u2_c					newSize			= Count - offset;
			if(count != (u2_t)-1)
				if_true_fef(count > newSize, GPK_FMT_GT_U2, count, (u2_t)newSize);
			out						= {Data ? &Data[offset] : 0, ::gpk::min(newSize, count)};
			rtrn out.size();
		}
		err_t					revert			()																{
			u2_c					lastElement		= Count - 1;
			for(u2_t i = 0, swapCount = Count / 2; i < swapCount; ++i) {
				T							old				= Data[i];
				Data[i]					= Data[lastElement - i];
				Data[lastElement - i]	= old;
			}
			rtrn 0;
		}

		inln	err_t			fill			(cnst T & value, u2_t offset = 0, u2_t stop = 0xFFFFFFFFU)										{ u2_c offsetStart = offset; for(; offset < ::gpk::min(Count, stop); ++offset) Data[offset] = value; rtrn offset - offsetStart; }

		err_t					for_each		(cnst TFuncForEach       <T> & funcForEach, u2_t offset = 0)								{ u2_c offsetStart = offset; for(; offset < Count; ++offset) if_fail_fef(funcForEach(Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					for_each		(cnst TFuncForEachConst  <T> & funcForEach, u2_t offset = 0)						cnst	{ u2_c offsetStart = offset; for(; offset < Count; ++offset) if_fail_fef(funcForEach(Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					enumerate		(cnst TFuncEnumerate     <T> & funcForEach, u2_t offset = 0)								{ u2_c offsetStart = offset; for(; offset < Count; ++offset) if_fail_fef(funcForEach(offset, Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					enumerate		(cnst TFuncEnumerateConst<T> & funcForEach, u2_t offset = 0)						cnst	{ u2_c offsetStart = offset; for(; offset < Count; ++offset) if_fail_fef(funcForEach(offset, Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		//
		err_t					for_each		(cnst TFuncForEach       <T> & funcForEach, u2_t offset, u2_t stop)						{ u2_c offsetStart = offset; for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) if_fail_fef(funcForEach(Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					for_each		(cnst TFuncForEachConst  <T> & funcForEach, u2_t offset, u2_t stop)				cnst	{ u2_c offsetStart = offset; for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) if_fail_fef(funcForEach(Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					enumerate		(cnst TFuncEnumerate     <T> & funcForEach, u2_t offset, u2_t stop)						{ u2_c offsetStart = offset; for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) if_fail_fef(funcForEach(offset, Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }
		err_t					enumerate		(cnst TFuncEnumerateConst<T> & funcForEach, u2_t offset, u2_t stop)				cnst	{ u2_c offsetStart = offset; for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) if_fail_fef(funcForEach(offset, Data[offset]), "offset:%u.", offset); rtrn offset - offsetStart; }

		err_t					find			(cnst FBool<T&>		& funcForEach	, u2_t offset = 0)											{ for(; offset < Count; ++offset) if(funcForEach(Data[offset])) rtrn (err_t)offset; rtrn -1; }
		err_t					find			(cnst FBool<TCnst&>	& funcForEach	, u2_t offset = 0)									cnst	{ for(; offset < Count; ++offset) if(funcForEach(Data[offset])) rtrn (err_t)offset; rtrn -1; }
		err_t					find			(cnst T				& value			, u2_t offset = 0)									cnst	{ for(; offset < Count; ++offset) if(Data[offset] == value) rtrn (err_t)offset; rtrn -1; }
		err_t					find			(TCstV sequence, u2_t offset = 0)										cnst	{
			if(sequence.size() > Count || offset > Count - sequence.size())
				rtrn -1;
			if(0 == sequence.size())
				rtrn offset;
			for(u2_t iOffset = offset, offsetStop = Count - sequence.size() + 1; iOffset < offsetStop; ++iOffset) {
				bool					equal			= true;
				for(u2_t iSequence = 0; iSequence < sequence.size(); ++iSequence)
					if(sequence[iSequence] != Data[iOffset + iSequence]) {
						equal					= false;
						break;
					}
				if(equal)
					rtrn iOffset;
			}
			rtrn -1;
		}
		err_t					rfind			(cnst T & value, u2_t offset = 0)										cnst	{
			if(offset >= Count)
				rtrn -1;
			for(u2_t iOffset = Count - 1 - offset; iOffset < Count; --iOffset)
				if(Data[iOffset] == value)
					rtrn iOffset;
			rtrn -1;
		}
		err_t					rfind			(TCstV sequence, u2_t offset = 0)										cnst	{
			if(sequence.size() > Count || offset > Count - sequence.size())
				rtrn -1;
			if(0 == sequence.size())
				rtrn Count - offset;
			for(u2_t iOffset = Count - sequence.size() - offset; iOffset < Count; --iOffset) {
				bool					equal			= true;
				for(u2_t iSequence = 0; iSequence < sequence.size(); ++iSequence)
					if(sequence[iSequence] != Data[iOffset + iSequence]) {
						equal					= false;
						break;
					}
				if(equal)
					rtrn iOffset;
			}
			rtrn -1;
		}

		tplt<tpnm _tMax> err_t	max				(_tMax & maxFound, cnst FTransform<_tMax, TCnst &> & funcComparand, u2_t offset = 0)	cnst	{
			if_true_fef(offset >= Count, GPK_FMT_GE_U2, offset, Count);
			err_t					iMax			= (err_t)offset;
			maxFound								= funcComparand(Data[offset]);
			for(++offset; offset < Count; ++offset) {
				_tMax					value			= funcComparand(Data[offset]);
				if(value > maxFound) {
					iMax								= (err_t)offset;
					maxFound							= value;
				}
			}
			rtrn iMax;
		}
		tplt<tpnm _tMax> err_t	min				(_tMax & minFound, cnst FTransform<_tMax, TCnst &> & funcComparand, u2_t offset = 0)	cnst	{
			if_true_fef(offset >= Count, GPK_FMT_GE_U2, offset, Count);
			err_t					iMin			= (err_t)offset;
			minFound								= funcComparand(Data[offset]);
			for(++offset; offset < Count; ++offset) {
				_tMax					value			= funcComparand(Data[offset]);
				if(value < minFound) {
					iMin								= (err_t)offset;
					minFound							= value;
				}
			}
			rtrn iMin;
		}
		tplt<tpnm _tMax> err_t	max				(cnst FTransform<_tMax, TCnst &> & funcComparand, u2_t offset = 0)						cnst	{ _tMax maxFound = {}; rtrn max(maxFound, funcComparand, offset); }
		tplt<tpnm _tMax> err_t	min				(cnst FTransform<_tMax, TCnst &> & funcComparand, u2_t offset = 0)						cnst	{ _tMax minFound = {}; rtrn min(minFound, funcComparand, offset); }
	}; // view<>

	tplTusng	view_array	= view<T>;
	tplTusng	view1d		= view<T>;
	tplTusng	v1			= view<T>;

	tplTnsix	u2_t		size			(cnst view<T> & viewToTest)														nxpt	{ rtrn viewToTest.size();		}
	tplTnsix	u2_t		byte_count		(cnst view<T> & viewToTest)														nxpt	{ rtrn viewToTest.byte_count();	}

#pragma pack(pop)

	tydf	view<b8_t>	vb8_t;	tdcs	 vb8_t 	vb8_c;
	tydf	view<uc_t>	vuc_t;	tdcs	 vuc_t 	vuc_c;
	tydf	view<sc_t>	vsc_t;	tdcs	 vsc_t 	vsc_c;
	tydf	view<u0_t>	vu0_t;	tdcs	 vu0_t 	vu0_c;
	tydf	view<u1_t>	vu1_t;	tdcs	 vu1_t 	vu1_c;
	tydf	view<u2_t>	vu2_t;	tdcs	 vu2_t 	vu2_c;
	tydf	view<u3_t>	vu3_t;	tdcs	 vu3_t 	vu3_c;
	tydf	view<s0_t>	vs0_t;	tdcs	 vs0_t 	vs0_c;
	tydf	view<s1_t>	vs1_t;	tdcs	 vs1_t 	vs1_c;
	tydf	view<s2_t>	vs2_t;	tdcs	 vs2_t 	vs2_c;
	tydf	view<s3_t>	vs3_t;	tdcs	 vs3_t 	vs3_c;
	tydf	view<f2_t>	vf2_t;	tdcs	 vf2_t 	vf2_c;
	tydf	view<f3_t>	vf3_t;	tdcs	 vf3_t 	vf3_c;
	
	tydf	view<b8_c>	vcb8_t;	tdcs	vcb8_t	vcb8_c;
	tydf	view<uc_c>	vcuc_t;	tdcs	vcuc_t	vcuc_c;
	tydf	view<sc_c>	vcsc_t;	tdcs	vcsc_t	vcsc_c;
	tydf	view<u0_c>	vcu0_t;	tdcs	vcu0_t	vcu0_c;
	tydf	view<u1_c>	vcu1_t;	tdcs	vcu1_t	vcu1_c;
	tydf	view<u2_c>	vcu2_t;	tdcs	vcu2_t	vcu2_c;
	tydf	view<u3_c>	vcu3_t;	tdcs	vcu3_t	vcu3_c;
	tydf	view<s0_c>	vcs0_t;	tdcs	vcs0_t	vcs0_c;
	tydf	view<s1_c>	vcs1_t;	tdcs	vcs1_t	vcs1_c;
	tydf	view<s2_c>	vcs2_t;	tdcs	vcs2_t	vcs2_c;
	tydf	view<s3_c>	vcs3_t;	tdcs	vcs3_t	vcs3_c;
	tydf	view<f2_c>	vcf2_t;	tdcs	vcf2_t	vcf2_c;
	tydf	view<f3_c>	vcf3_t;	tdcs	vcf3_t	vcf3_c;
	
	tydf	view<vuc_t>	vvuc_t ;	tdcs	vvuc_t	vvuc_c;
	tydf	view<vsc_t>	vvsc_t ;	tdcs	vvsc_t	vvsc_c;
	tydf	view<vu0_t>	vvu0_t ;	tdcs	vvu0_t	vvu0_c;
	tydf	view<vu1_t>	vvu1_t ;	tdcs	vvu1_t	vvu1_c;
	tydf	view<vu2_t>	vvu2_t ;	tdcs	vvu2_t	vvu2_c;
	tydf	view<vu3_t>	vvu3_t ;	tdcs	vvu3_t	vvu3_c;
	tydf	view<vs0_t>	vvs0_t ;	tdcs	vvs0_t	vvs0_c;
	tydf	view<vs1_t>	vvs1_t ;	tdcs	vvs1_t	vvs1_c;
	tydf	view<vs2_t>	vvs2_t ;	tdcs	vvs2_t	vvs2_c;
	tydf	view<vs3_t>	vvs3_t ;	tdcs	vvs3_t	vvs3_c;
	tydf	view<vf2_t>	vvf2_t ;	tdcs	vvf2_t	vvf2_c;
	tydf	view<vf3_t>	vvf3_t ;	tdcs	vvf3_t	vvf3_c;
	tydf	view<vuc_c>	vcvuc_t;	tdcs	vcvuc_t	vcvuc_c;
	tydf	view<vsc_c>	vcvsc_t;	tdcs	vcvsc_t	vcvsc_c;
	tydf	view<vu0_c>	vcvu0_t;	tdcs	vcvu0_t	vcvu0_c;
	tydf	view<vu1_c>	vcvu1_t;	tdcs	vcvu1_t	vcvu1_c;
	tydf	view<vu2_c>	vcvu2_t;	tdcs	vcvu2_t	vcvu2_c;
	tydf	view<vu3_c>	vcvu3_t;	tdcs	vcvu3_t	vcvu3_c;
	tydf	view<vs0_c>	vcvs0_t;	tdcs	vcvs0_t	vcvs0_c;
	tydf	view<vs1_c>	vcvs1_t;	tdcs	vcvs1_t	vcvs1_c;
	tydf	view<vs2_c>	vcvs2_t;	tdcs	vcvs2_t	vcvs2_c;
	tydf	view<vs3_c>	vcvs3_t;	tdcs	vcvs3_t	vcvs3_c;
	tydf	view<vf2_c>	vcvf2_t;	tdcs	vcvf2_t	vcvf2_c;
	tydf	view<vf3_c>	vcvf3_t;	tdcs	vcvf3_t	vcvf3_c;
	
	struct view_const_string : view<sc_c> {
		inxp				view_const_string		()										nxpt	= dflt;
		inxp				view_const_string		(cnst view<sc_t> & other)				nxpt	: view(other)			{}
		inxp				view_const_string		(cnst view<sc_c> & other)				nxpt	: view(other)			{}
							view_const_string		(sc_c * storage, u2_t length)					: view(storage, length)	{ Count = (length == (u2_t)-1) ? (u2_t)strlen(storage) : length; }
		tplN2u		inxp	view_const_string		(sc_c (&storage)[N], u2_t length = -1)	nxpt	: view(storage)			{
			for(Count = 0; Count < ::gpk::min(N, length); ++Count)
				if(0 == storage[Count])
					break;
		}

#ifdef GPK_ARDUINO
#   ifdef GPK_ESP8266
        ndin		oper		String			()  cnst   { return String(Data); }
#   else
        ndin		oper		String			()  cnst   { return String(Data, Count); }
#   endif
#endif
		ndix		b8_t		oper==			(view_const_string other)	csnx	{ rtrn view<sc_c>::oper==(other); }
		ndix		b8_t		oper!=			(view_const_string other)	csnx	{ rtrn !oper==(other); }
		tplN2undix	b8_t		oper==			(sc_c (&other)[N])			csnx	{ rtrn view<sc_c>::oper==(view_const_string(other)); }
		tplN2undix	b8_t		oper!=			(sc_c (&other)[N])			csnx	{ rtrn !oper==(other); }

		ndix		sc_c*		begin			()							csnx	{ rtrn (Data && Count) ? Data : ""; }
		ndix		sc_c*		end				()							csnx	{ rtrn (Data && Count) ? Data + Count : begin(); }
		ndix		oper		sc_c* 			()							csnx	{ rtrn begin(); }
	};
	//tplN2u	bool	view_string	::oper==	(sc_c (&other)[N])	cnst	{ rtrn view<sc_t>::operator==(view_const_string(other)); }
	tydf	view_const_string	vcst_t;
	tdcs	vcst_t				vcst_c;

	struct view_string : view<sc_t> {
		inxp				view_string			()										nxpt	= dflt;
		inln				view_string			(cnst view<sc_t> & other)				nxpt	: view(other)			{}
							view_string			(sc_t * storage, u2_t length)					: view(storage, length)	{ Count = (length == (u2_t)-1) ? (u2_t)strlen(storage) : length; }
		tplN2u				view_string			(sc_t (&storage)[N], u2_t length = -1)	nxpt	: view(storage)			{
			for(Count = 0; Count < ::gpk::min(N, length); ++Count)
				if(0 == storage[Count])
					break;
		}

		ndin		oper	vcst_t				()					csnx	{ rtrn {Data, Count}; };

		ndin		b8_t	oper==				(vcst_t other)		csnx	{ rtrn view<sc_t>::oper==(other); }
		ndin		b8_t	oper!=				(vcst_t other)		csnx	{ rtrn !oper==(other); }
		tplN2undix	b8_t	oper==				(sc_c (&other)[N])	csnx	{ rtrn view<sc_t>::oper==(view_const_string(other)); }
		tplN2undix	b8_t	oper!=				(sc_c (&other)[N])	csnx	{ rtrn !oper==(other); }

		ndix		sc_c*	begin				()					csnx	{ rtrn (Data && Count) ? Data : ""; }
		ndix		sc_c*	end					()					csnx	{ rtrn (Data && Count) ? Data + Count : begin(); }
		ndix		oper	sc_c* 				()					csnx	{ rtrn begin(); }
		ndin		sc_t*	begin				()					nxpt	{ rtrn Data; }
		ndin		sc_t*	end					()					nxpt	{ rtrn begin() + Count; }
		ndin		oper	sc_t* 				()					nxpt	{ rtrn begin(); }

	};
	tydf	view_string		vstr_t;
	tdcs	vstr_t			vstr_c;
#define GPK_VIEW_ARGS(view) (int)view.size(), view.begin() // Parenthesizing the argument does not merely tolerate expressions—it legitimizes and conceals them. Without those parentheses, the macro enforces its intended use.

	tplt<tpnm _tVal>		using kvvcst_t	= kv<vcst_t, _tVal>;
	stxp		vcst_t	VCC_NULL		= "null";
	stxp		vcst_t	VCC_TRUE		= "true";
	stxp		vcst_t	VCC_FALSE		= "false";

	nsix		vcst_t	b82st			(bool b)					nxpt	{ rtrn b ? VCC_TRUE : VCC_FALSE; }
	nsix		u0_t	b82u0			(bool b)					nxpt	{ rtrn b ? 1 : 0; }
	nsix		u0_t	b82s0			(bool b)					nxpt	{ rtrn b ? 1 : 0; }
	ndsi		sc_c*	b82sc			(bool b)					nxpt	{ rtrn b ? VCC_TRUE : VCC_FALSE; }
	stin		sc_c*	b82sc			(bool b, vcst_t & output)	nxpt	{ rtrn output = b ? VCC_TRUE : VCC_FALSE; }
	stin		bool	cst2b8			(vcst_t b)					nxpt	{ rtrn b.size() && b != VCC_FALSE; }

	stin		vstr_t	str				(vstr_t & arg)				nxpt	{ rtrn arg; }
	stin		vcst_t	str				(cnst vstr_t & arg)			nxpt	{ rtrn arg; }
	stin		vcst_t	str				(cnst vcst_t & arg)			nxpt	{ rtrn arg; }
	stin		vstr_t	str				(vs0_t arg)					nxpt	{ rtrn arg; }
	stin		vcst_t	str				(vcs0_t arg)				nxpt	{ rtrn arg; }
	tplN2usinx	vstr_t	str				(sc_t (&arg)[N])			nxpt	{ rtrn arg; }
	tplN2usinx	vcst_t	str				(sc_c (&arg)[N])			nxpt	{ rtrn arg; }
	sinx		vcst_t	str				(cnst bool arg)				nxpt	{ rtrn arg ? VCC_TRUE : VCC_FALSE; }
	//
	tplTnsix	vcst_t	get_type_namev	()							nxpt	{ rtrn "unknown"; }
	tplTnsix	sc_c*	get_type_namep	()							nxpt	{ rtrn get_type_namev<T>().begin(); }

#define GDEFINE_TYPE_NAME_STR(typeIdentifier)																										\
			nsix	::gpk::vcst_t	get_type_namev					(typeIdentifier &)	nxpt	{ rtrn #typeIdentifier; }					\
	tplt<>	ndix	::gpk::vcst_t	get_type_namev<typeIdentifier>	()					nxpt	{ rtrn #typeIdentifier; }					\
			nsix	::gpk::sc_c*	get_type_namep					(typeIdentifier &)	nxpt	{ rtrn get_type_namev<typeIdentifier>().begin(); }	\
	tplt<>	ndix	::gpk::sc_c*	get_type_namep<typeIdentifier>	()					nxpt	{ rtrn get_type_namev<typeIdentifier>().begin(); }

	GDEFINE_TYPE_NAME_STR(b8_t);
	GDEFINE_TYPE_NAME_STR(sc_t);
	GDEFINE_TYPE_NAME_STR(u0_t);
	GDEFINE_TYPE_NAME_STR(u1_t);
	GDEFINE_TYPE_NAME_STR(u2_t);
	GDEFINE_TYPE_NAME_STR(u3_t);
	GDEFINE_TYPE_NAME_STR(s1_t);
	GDEFINE_TYPE_NAME_STR(s2_t);
	GDEFINE_TYPE_NAME_STR(s3_t);
	GDEFINE_TYPE_NAME_STR(f2_t);
	GDEFINE_TYPE_NAME_STR(f3_t);
	GDEFINE_TYPE_NAME_STR(b8_c);
	GDEFINE_TYPE_NAME_STR(sc_c);
	GDEFINE_TYPE_NAME_STR(u0_c);
	GDEFINE_TYPE_NAME_STR(u1_c);
	GDEFINE_TYPE_NAME_STR(u2_c);
	GDEFINE_TYPE_NAME_STR(u3_c);
	GDEFINE_TYPE_NAME_STR(s1_c);
	GDEFINE_TYPE_NAME_STR(s2_c);
	GDEFINE_TYPE_NAME_STR(s3_c);
	GDEFINE_TYPE_NAME_STR(f2_c);
	GDEFINE_TYPE_NAME_STR(f3_c);
	//
	tydf	view<vstr_t>	vvstr_t;
	tydf	view<vcst_t>	vvcst_t;
	tydf	view<vstr_c>	vcvstr_t;
	tydf	view<vcst_c>	vcvcst_t;

	stxp	vcst_t		TRIM_CHARACTERS		= " \t\b\n\r";

	err_t			rtrim				(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters = TRIM_CHARACTERS);
	err_t			ltrim				(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters = TRIM_CHARACTERS);
	err_t			trim				(vcst_t & trimmed, cnst vcst_t & original, cnst vcst_t & characters = TRIM_CHARACTERS);
	stin	err_t	rtrim				(vcst_t & trimmed) 	{ rtrn rtrim(trimmed, trimmed); }
	stin	err_t	ltrim				(vcst_t & trimmed) 	{ rtrn ltrim(trimmed, trimmed); }
	stin	err_t	trim				(vcst_t & trimmed) 	{ rtrn trim(trimmed, trimmed); }

	stin	err_t	rtrim				(vs0_t & trimmed, cnst vs0_t & original, vcst_t characters = TRIM_CHARACTERS)	{ vcst_t result = {}; cnst err_t count = rtrim(result, {original.begin(), original.size()}, characters); trimmed = {(sc_t*)result.begin(), result.size()}; rtrn count; }
	stin	err_t	ltrim				(vs0_t & trimmed, cnst vs0_t & original, vcst_t characters = TRIM_CHARACTERS)	{ vcst_t result = {}; cnst err_t count = ltrim(result, {original.begin(), original.size()}, characters); trimmed = {(sc_t*)result.begin(), result.size()}; rtrn count; }
	stin	err_t	trim				(vs0_t & trimmed, cnst vs0_t & original, vcst_t characters = TRIM_CHARACTERS)	{ vcst_t result = {}; cnst err_t count = trim(result, {original.begin(), original.size()}, characters); trimmed = {(sc_t*)result.begin(), result.size()}; rtrn count; }
	stin	err_t	rtrim				(vs0_t & trimmed) 	{ rtrn rtrim(trimmed, trimmed); }
	stin	err_t	ltrim				(vs0_t & trimmed) 	{ rtrn ltrim(trimmed, trimmed); }
	stin	err_t	trim				(vs0_t & trimmed) 	{ rtrn trim(trimmed, trimmed); }

	tplT	err_t	reverse				(view<T> elements)													{
		u2_c				lastElement			= elements.size() - 1;
		for(u2_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			T							old						= elements[i];
			elements[i]					= elements[lastElement - i];
			elements[lastElement - i]	= old;
		}
		rtrn 0;
	}

	tplT	err_t	find					(cnst T & valueToFind, view<cnst T> target, u2_t offset = 0)		{ rtrn target.find(valueToFind, offset); }

	tplT	err_t	rfind					(cnst T & valueToFind, view<cnst T> target, u2_t offset = 0)		{ rtrn target.rfind(valueToFind, offset); }
	tplt<tpnm _tKey, tpnm _tVal>
	err_t			find					(cnst gpk_rmcnst(_tKey) & keyToFind, view<cnst keyval<_tKey, _tVal>> keyvals) {
		for(u2_t iKeyVal = 0; iKeyVal < keyvals.size(); ++iKeyVal)
			if(keyToFind == keyvals[iKeyVal].Key)
				rtrn iKeyVal;
		rtrn -1;
	}
	tplt<tpnm _tKey, tpnm _tVal>
	err_t			find					(cnst gpk_rmcnst(_tKey) & keyToFind, view<cnst keyval<_tKey, _tVal>> keyvals, gpk_rmcnst(_tVal) & out_val) {
		cnst err_t		index						= find(keyToFind, keyvals);
		out_val		= 0 > index ? gpk_rmcnst(_tVal){} : keyvals[index].Val;
		rtrn index;
	}
	stin	err_t	find_string				(vcst_t toFind, vcst_t target, u2_t offset = 0) { rtrn target.find(toFind, offset); }
	stin	err_t	rfind_string			(vcst_t toFind, vcst_t target, u2_t offset = 0) { rtrn target.rfind(toFind, offset); }

	tplT	err_t	split					(tpnm view<T>::TCnst & valueToFind, view<T> & input) {
		cnst err_t			iValue					= find<gpk_rmcnst(T)>(valueToFind, input);
		rtrn input.slice(input, 0, (u2_t)iValue);
	}

	tplT	err_t	split					(tpnm view<T>::TCnst & valueToFind, view<T> original, view<T> & left, view<T> & right) {
		left			= original;
		if_fail_fe(split(valueToFind, left));
		if(left.size() == original.size()) {
			right			= {};
			rtrn -1;
		}
		if_fail_fe(original.slice(right, left.size() + 1));
		rtrn left.size();
	}

	tplT requires(false == is_cnst<T>::Value)
	err_t			split					(tpnm view<T>::TCnst & valueToFind, view<T> original, tpnm view<T>::TConstView & left, tpnm view<T>::TConstView & right) {
		tpnm view<T>::TConstView	constOriginal			= original;
		rtrn split(valueToFind, constOriginal, left, right);
	}

	tplT	err_t	splitAt					(tpnm view<T>::TCnst & valueToFind, view<T> original, view<T> & left, view<T> & right) {
		cnst err_t			iValue					= original.find(valueToFind);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left			= original;
			right			= {};
		}
		else {
			if_fail_fe(original.slice(left, 0, (u2_t)iValue));
			if_fail_fe(original.slice(right, iValue, (u2_t)original.size() - iValue));
		}
		rtrn iValue;
	}

	tplT requires(false == is_cnst<T>::Value)
	err_t			splitAt					(tpnm view<T>::TCnst & valueToFind, view<T> original, tpnm view<T>::TConstView & left, tpnm view<T>::TConstView & right) {
		tpnm view<T>::TConstView	constOriginal			= original;
		rtrn splitAt(valueToFind, constOriginal, left, right);
	}

	// Returns the index of the start of the sequence if the latter found.
	tplT	err_t	split					(cnst view<T> & sequenceToFind, view<T> original, view<T> & left, view<T> & right) {
		cnst err_t			iValue					= original.find(sequenceToFind);
		if(0 > iValue) {
			left			= original;
			right			= {};
		}
		else {
			if_fail_fe(original.slice(right, iValue + sequenceToFind.size()));
			if_fail_fe(original.slice(left, 0, iValue));
		}
		rtrn iValue;
	}

	tplT requires(false == is_cnst<T>::Value)
	err_t			split					(cnst view<T> & sequenceToFind, view<T> original, tpnm view<T>::TConstView & left, tpnm view<T>::TConstView & right) {
		tpnm view<T>::TConstView	constSequence			= sequenceToFind;
		tpnm view<T>::TConstView	constOriginal			= original;
		rtrn split(constSequence, constOriginal, left, right);
	}

	tplt<tpnm T>
	inln	err_t	split					(cnst view<T> & sequenceToFind, view<T> & inputOrLeft, view<T> & right) {
		rtrn split(sequenceToFind, inputOrLeft, inputOrLeft, right);
	}

	tplT	err_t	splitAt					(cnst view<T> & sequenceToFind, view<T> original, view<T> & left, view<T> & right) {
		cnst err_t			iValue					= original.find(sequenceToFind);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left							= original;
			right							= {};
		}
		else {
			if_fail_fef(original.slice(left, 0, (u2_t)iValue), "%s", "Invalid slice");
			if_fail_fef(original.slice(right, iValue, (u2_t)original.size() - iValue), "%s", "Invalid slice");
		}
		rtrn iValue;
	}

	tplT requires(false == is_cnst<T>::Value)
	err_t			splitAt					(cnst view<T> & sequenceToFind, view<T> original, tpnm view<T>::TConstView & left, tpnm view<T>::TConstView & right) {
		tpnm view<T>::TConstView	constSequence			= sequenceToFind;
		tpnm view<T>::TConstView	constOriginal			= original;
		rtrn splitAt(constSequence, constOriginal, left, right);
	}

	tplT	err_t	max						(view<T> input, T ** result) {
		if_true_fef(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result				= &input[0];
		s2_t					iMax					= 0;
		for(u2_t iElement = 1; iElement < input.size(); ++iElement) {
			cnst T					& currentElement		= input[iElement];
			if(currentElement > **result) {
				*result					= &currentElement;
				iMax					= iElement;
			}
		}
		rtrn iMax;
	}

	tplT	err_t	min						(view<T> input, T ** result) {
		if_true_fef(0 == input.size(), "%s", "Cannot get reference to min element of an empty array.");
		*result				= &input[0];
		s2_t					iMin					= 0;
		for(u2_t iElement = 1; iElement < input.size(); ++iElement) {
			cnst T					& currentElement		= input[iElement];
			if(currentElement < **result) {
				*result				= &currentElement;
				iMin				= iElement;
			}
		}
		rtrn iMin;
	}

	tplT		T&	max						(view<T> elements)		{ T * rmax	{}; if_fail_e(max(elements, &rmax));	rtrn *rmax; }
	tplT		T&	min						(view<T> elements)		{ T * rmin	{}; if_fail_e(min(elements, &rmin));	rtrn *rmin; }
	tplT		T	sum						(view<cnst T> elements)	{ T result	{}; for(T element : elements) result += element; rtrn result; }
	tplTstin	T&	be2le					(T & number)			{ reverse<u0_t>({(u0_t*)&number, sizeof(T)}); rtrn number; }

#define GPK_USING_VIEW()												\
	usng	::gpk::vb8_t, ::gpk::vb8_c, ::gpk::vcb8_t, ::gpk::vcb8_c	\
		,	::gpk::vuc_t, ::gpk::vuc_c, ::gpk::vcuc_t, ::gpk::vcuc_c	\
		,	::gpk::vsc_t, ::gpk::vsc_c, ::gpk::vcsc_t, ::gpk::vcsc_c	\
		,	::gpk::vu0_t, ::gpk::vu0_c, ::gpk::vcu0_t, ::gpk::vcu0_c	\
		,	::gpk::vu1_t, ::gpk::vu1_c, ::gpk::vcu1_t, ::gpk::vcu1_c	\
		,	::gpk::vu2_t, ::gpk::vu2_c, ::gpk::vcu2_t, ::gpk::vcu2_c	\
		,	::gpk::vu3_t, ::gpk::vu3_c, ::gpk::vcu3_t, ::gpk::vcu3_c	\
		,	::gpk::vs0_t, ::gpk::vs0_c, ::gpk::vcs0_t, ::gpk::vcs0_c	\
		,	::gpk::vs1_t, ::gpk::vs1_c, ::gpk::vcs1_t, ::gpk::vcs1_c	\
		,	::gpk::vs2_t, ::gpk::vs2_c, ::gpk::vcs2_t, ::gpk::vcs2_c	\
		,	::gpk::vs3_t, ::gpk::vs3_c, ::gpk::vcs3_t, ::gpk::vcs3_c	\
		,	::gpk::vf2_t, ::gpk::vf2_c, ::gpk::vcf2_t, ::gpk::vcf2_c	\
		,	::gpk::vf3_t, ::gpk::vf3_c, ::gpk::vcf3_t, ::gpk::vcf3_c	\
		,	::gpk::vstr_t, ::gpk::vcst_t, ::gpk::vstr_c, ::gpk::vcst_c

} // namespace

#endif // GPK_ARRAY_VIEW_H_23627
