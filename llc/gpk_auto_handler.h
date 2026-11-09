#include "gpk_typeint.h"

/// Copyright 2017 - asm128
#ifndef GPK_AUTO_HANDLER_H_23627
#define GPK_AUTO_HANDLER_H_23627

namespace gpk
{
	 // I have no idea how to call this type so I'm really sorry. Something about raii maybe?
	tplt<tpnm _tHandle, _tHandle _vDefault>
	struct auto_handler {
		typedef	auto_handler<_tHandle, _vDefault>	TWrapper;

				_tHandle							Handle							= _vDefault;
		stacxpr	const _tHandle						VDefault						= _vDefault;

		inxp										auto_handler					()								noexcept	= default;
		inxp										auto_handler					(const _tHandle & other)		noexcept	= delete;
		inxp										auto_handler					(const TWrapper & other)		noexcept	= delete;

		inxp	operator							const _tHandle&					()						const	noexcept	{ return Handle;		}

		inxp	TWrapper							operator=		(const _tHandle & other)		noexcept	= delete;
		inxp	_tHandle*							operator&						()								noexcept	{ return &Handle;		}
#if !defined(GPK_ATMEL)
		inxp	const _tHandle*						operator&						()						const	noexcept	{ return &Handle;		}
#endif
	};
} // namespace

#endif // GPK_AUTO_HANDLER_H_23627
