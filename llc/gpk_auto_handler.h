#include "gpk_typeint.h"

/// Copyright 2017-2024 ogarnd
#ifndef GPK_AUTO_HANDLER_H_23627
#define GPK_AUTO_HANDLER_H_23627

nmsp gpk
{
	 // I have no idea how to call this type so I'm really sorry. Something about raii maybe?
	tplt<tpnm _tHandle, _tHandle _vDefault>
	struct auto_handler {
		tydf	auto_handler<_tHandle, _vDefault>	TWrapper;

				_tHandle							Handle							= _vDefault;
		stxp	cnst _tHandle						VDefault						= _vDefault;

		inxp										auto_handler					()								nxpt	= default;
		inxp										auto_handler					(cnst _tHandle & other)		nxpt	= delete;
		inxp										auto_handler					(cnst TWrapper & other)		nxpt	= delete;

		inxp	oper							cnst _tHandle&					()						csnx	{ return Handle;		}

		inln	TWrapper							oper=						(cnst _tHandle & other)		nxpt	= delete;
		inln	_tHandle*							oper&						()								nxpt	{ return &Handle;		}
#if !defined(GPK_ATMEL)
		inxp	cnst _tHandle*						oper&						()						csnx	{ return &Handle;		}
#endif
	};
} // nmsp

#endif // GPK_AUTO_HANDLER_H_23627
