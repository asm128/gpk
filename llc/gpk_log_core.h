#include "gpk_error.h"

#ifdef GPK_ATMEL
#	define GPK_LOG_ARDUINO_FLASHSTRINGHELPER
#endif

#ifdef GPK_LOG_ARDUINO_FLASHSTRINGHELPER
#	include <WString.h>
#endif

#ifndef GPK_LOG_CORE_H
#define GPK_LOG_CORE_H

namespace gpk
{
	typedef	void	(*log_print_t)		(const char * text);
	void			_base_log_print		(const char * text);
	typedef	void	(*log_write_t)		(const char * text, uint32_t textLen);
	void			_base_log_write		(const char * text, uint32_t textLen);
#define base_log_write(text, textLen)	::gpk::_base_log_write(text, textLen)
#define base_log_print(text)			::gpk::_base_log_print(text)

#ifdef GPK_LOG_ARDUINO_FLASHSTRINGHELPER
	typedef	void	(*log_print_P_t)	(const __FlashStringHelper * text);
	void			_base_log_print_P	(const __FlashStringHelper * text);
#	define base_log_print_P(text)	::gpk::_base_log_print_P(text)
#	define base_log_print_F(text)	base_log_print_P(F(text))
#else
#	define base_log_print_P(text)	base_log_print(text)
#	define base_log_print_F(text)	base_log_print(text)
#endif

	tplt<tpnm... _tArgs>
	stincxp	void	dummy				(_tArgs&&...)		{}
} // namespace

#ifdef GPK_WINDOWS
#	define gpk_throw(...)								throw(__VA_ARGS__)
#else
#	define gpk_throw(...)								do { char * nulp = 0; unsigned int i = 0; while(++i) nulp[i] = (char)i; ::gpk::dummy(__VA_ARGS__); } while(0)
#endif

#ifndef block_log
#	define block_log(gpk_logFunction, statement, textToLog) { gpk_logFunction(textToLog); statement; }	// Block with a statement preceded by a log call.
#	ifdef GPK_WINDOWS
#		define block_logf(gpk_logFunction, statement, format, ...) { gpk_logFunction(format, __VA_ARGS__); statement; }	// Block with a statement preceded by a formatted log call.
#	else
#		define block_logf(gpk_logFunction, statement, format, ...) { gpk_logFunction(format, ##__VA_ARGS__); statement; }	// Block with a statement preceded by a formatted log call.
#	endif
#endif

// - Conditional log of condition.
#ifndef if_true_log
#	define if_true_log(gpk_logFunction, condition)									if(condition) { gpk_logFunction(#condition); }											// - Log condition if (condition) == true.
#	define if_true_block_log(gpk_logFunction, condition, statement)					if(condition) block_log(gpk_logFunction, statement, #condition)							// - Conditional block with custom statement preceded by a log call.
#endif // if_true_log

// - Conditional log formatted.
#ifndef if_true_logf
#	ifdef GPK_WINDOWS					
#		define if_true_logf(gpk_logFunction, condition, format, ...)						if(condition) { gpk_logFunction(#condition "-> " format, __VA_ARGS__); }					// - Log condition and custom formatted string args if (condition) == true.
#		define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)		if(condition) block_logf(gpk_logFunction, statement, #condition "-> " format, __VA_ARGS__)	// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#	else
#		ifdef GPK_ATMEL																																																											
#			define if_true_logf(gpk_logFunction, condition, format, ...)					if_true_log			(gpk_logFunction, (condition))				// - Log condition and custom formatted string args if (condition) == true.
#			define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)	if_true_block_log	(gpk_logFunction, (condition), statement) 	// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#		else
#			define if_true_logf(gpk_logFunction, condition, format, ...)					if(condition) { gpk_logFunction(#condition "-> " format, ##__VA_ARGS__); }						// - Log condition and custom formatted string args if (condition) == true.
#			define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)	if(condition) block_logf(gpk_logFunction, statement, #condition "-> " format, ##__VA_ARGS__)		// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#		endif
#	endif
#endif // if_true_logf


#ifdef if_true_log
#	define if_true_log_and_break(gpk_logFunction, condition)						if_true_block_log(gpk_logFunction, (condition), break)						// - Log condition and break if (condition) == true.
#	define if_true_log_and_continue(gpk_logFunction, condition)						if_true_block_log(gpk_logFunction, (condition), continue)					// - Log condition and continue if (condition) == true.
#	define if_true_log_and_throw(gpk_logFunction, condition)						if_true_block_log(gpk_logFunction, (condition), gpk_throw(#condition))			// - Log condition and return if (condition) == true.
#	define if_true_log_and_return(gpk_logFunction, condition)						if_true_block_log(gpk_logFunction, (condition), return)						// - Log condition and return if (condition) == true.
#	define if_true_log_and_return_value(gpk_logFunction, valueToReturn, condition)	if_true_block_log(gpk_logFunction, (condition), return (valueToReturn))		// - Log condition and return some value if (condition) == true.
#endif // if_true_log
#ifdef if_true_logf
#	define if_true_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_block_logf(gpk_logFunction, (condition), break, format, __VA_ARGS__)			// - Log formatted string and break if (condition) == true.
#	define if_true_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_block_logf(gpk_logFunction, (condition), continue, format, __VA_ARGS__)			// - Log formatted string and continue if (condition) == true.
#	define if_true_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_block_logf(gpk_logFunction, (condition), gpk_throw(#condition), format, __VA_ARGS__)			// - Log condition and return if (condition) == true.
#	define if_true_logf_and_return(gpk_logFunction, condition, format, ...)					if_true_block_logf(gpk_logFunction, (condition), return, format, __VA_ARGS__)			// - Log formatted string and return if (condition) == true.
#	define if_true_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_block_logf(gpk_logFunction, (condition), return (value), format, __VA_ARGS__)	// - Log formatted string and return some value if (condition) == true.
#endif // if_true_log

#ifdef if_true_log
#	ifndef if_fail_log
#		define if_fail_log(gpk_logFunction, condition)									if_true_log					(gpk_logFunction, 0 > gpk::error_t(condition))				// - if condition < 0 -> log
#		define if_fail_log_and_break(gpk_logFunction, condition)						if_true_log_and_break		(gpk_logFunction, 0 > gpk::error_t(condition))				// - if condition < 0 -> break
#		define if_fail_log_and_continue(gpk_logFunction, condition)						if_true_log_and_continue	(gpk_logFunction, 0 > gpk::error_t(condition))				// - if condition < 0 -> continue
#		define if_fail_log_and_throw(gpk_logFunction, condition)						if_true_log_and_throw		(gpk_logFunction, 0 > gpk::error_t(condition))				// - if condition < 0 -> return
#		define if_fail_log_and_return(gpk_logFunction, condition)						if_true_log_and_return		(gpk_logFunction, 0 > gpk::error_t(condition))				// - if condition < 0 -> return
#		define if_fail_log_and_return_value(gpk_logFunction, value, condition)			if_true_log_and_return_value(gpk_logFunction, (value), 0 > gpk::error_t(condition))		// - if condition < 0 -> return value
#		define if_zero_log(gpk_logFunction, condition)									if_true_log					(gpk_logFunction, 0 == (condition))							// - if condition == 0 -> log
#		define if_zero_log_and_break(gpk_logFunction, condition)						if_true_log_and_break		(gpk_logFunction, 0 == (condition))							// - if condition == 0 -> break
#		define if_zero_log_and_continue(gpk_logFunction, condition)						if_true_log_and_continue	(gpk_logFunction, 0 == (condition))							// - if condition == 0 -> continue
#		define if_zero_log_and_throw(gpk_logFunction, condition)						if_true_logf_and_throw		(gpk_logFunction, 0 == (condition))							// - if condition == 0 -> return
#		define if_zero_log_and_return(gpk_logFunction, condition)						if_true_log_and_return		(gpk_logFunction, 0 == (condition))							// - if condition == 0 -> return
#		define if_zero_log_and_return_value(gpk_logFunction, value, condition)			if_true_log_and_return_value(gpk_logFunction, (value), 0 == (condition))				// - if condition == 0 -> return value
#		define if_null_log																if_zero_log																				// - if condition == 0 -> log
#		define if_null_log_and_break													if_zero_log_and_break																	// - if condition == 0 -> break
#		define if_null_log_and_continue													if_zero_log_and_continue																// - if condition == 0 -> continue
#		define if_null_log_and_throw													if_zero_log_and_throw																	// - if condition == 0 -> return
#		define if_null_log_and_return													if_zero_log_and_return																	// - if condition == 0 -> return
#		define if_null_log_and_return_value												if_zero_log_and_return_value															// - if condition == 0 -> return value
#	endif // if_fail_log
#endif // if_true_log

#ifdef if_true_logf
#	ifndef if_fail_logf
#		define if_fail_logf(gpk_logFunction, condition, format, ...)							if_true_logf					(gpk_logFunction, 0 > gpk::error_t(condition), format, __VA_ARGS__)				// - if condition < 0 -> log
#		define if_fail_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_logf_and_break			(gpk_logFunction, 0 > gpk::error_t(condition), format, __VA_ARGS__)				// - if condition < 0 -> break
#		define if_fail_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_logf_and_continue		(gpk_logFunction, 0 > gpk::error_t(condition), format, __VA_ARGS__)				// - if condition < 0 -> continue
#		define if_fail_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_logf_and_throw			(gpk_logFunction, 0 > gpk::error_t(condition), format, __VA_ARGS__)				// - if condition < 0 -> return
#		define if_fail_logf_and_return(gpk_logFunction, condition, format, ...)					if_true_logf_and_return			(gpk_logFunction, 0 > gpk::error_t(condition), format, __VA_ARGS__)				// - if condition < 0 -> return
#		define if_fail_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_logf_and_return_value	(gpk_logFunction, (value), 0 > gpk::error_t(condition), format, __VA_ARGS__)	// - if condition < 0 -> return value
#		define if_zero_logf(gpk_logFunction, condition, format, ...)							if_true_logf					(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)						// - if condition == 0 -> log
#		define if_zero_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_logf_and_break			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)						// - if condition == 0 -> break
#		define if_zero_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_logf_and_continue		(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)						// - if condition == 0 -> continue
#		define if_zero_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_logf_and_throw			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)						// - if condition == 0 -> return
#		define if_zero_logf_and_return(gpk_logFunction, condition, format, ...)					if_true_logf_and_return			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)						// - if condition == 0 -> return
#		define if_zero_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_logf_and_return_value	(gpk_logFunction, (value), 0 == (condition), format, __VA_ARGS__)				// - if condition == 0 -> return value
#		define if_null_logf						if_zero_logf																																					// - if condition == 0 -> log
#		define if_null_logf_and_break			if_zero_logf_and_break																																			// - if condition == 0 -> break
#		define if_null_logf_and_continue		if_zero_logf_and_continue																																		// - if condition == 0 -> continue
#		define if_null_log_and_throw			if_zero_log_and_throw																																			// - if condition == 0 -> return
#		define if_null_logf_and_return			if_zero_logf_and_return																																			// - if condition == 0 -> return
#		define if_null_logf_and_return_value	if_zero_logf_and_return_value																																	// - if condition == 0 -> return value
#	endif // if_fail_logf
#endif // if_true_logf

#endif // GPK_LOG_CORE_H
