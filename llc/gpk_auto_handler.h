#include "gpk_platform.h"

/// Copyright 2017 - asm128
#ifndef GPK_AUTO_HANDLER_H_0239840239840238321
#define GPK_AUTO_HANDLER_H_0239840239840238321

namespace gpk
{
	 // I have no idea how to call this type so I'm really sorry. Something about raii maybe?
	template<typename _tHandle, _tHandle _vDefault>
	struct auto_handler {
		typedef				auto_handler<_tHandle, _vDefault>				TWrapper;

							_tHandle										Handle							= _vDefault;
		static constexpr	const _tHandle									VDefault						= _vDefault;

		inline constexpr													auto_handler					()										noexcept	= default;
		inline constexpr													auto_handler					(const _tHandle& other)					noexcept	= delete;
		inline constexpr													auto_handler					(const TWrapper& other)					noexcept	= delete;

		inline constexpr	operator										const _tHandle&					()								const	noexcept	{ return Handle;		}

		inline constexpr	TWrapper										operator=						(const _tHandle& other)					noexcept	= delete;

		inline constexpr	_tHandle*										operator&						()										noexcept	{ return &Handle;		}
#if !defined(GPK_ATMEL)
		inline constexpr	const _tHandle*									operator&						()								const	noexcept	{ return &Handle;		}
#endif
	};
} // namespace

#endif // GPK_AUTO_HANDLER_H_0239840239840238321
