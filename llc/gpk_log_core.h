#include "gpk_typeint.h"

#ifndef GPK_LOG_CORE_H
#define GPK_LOG_CORE_H

#define block_log(gpk_logFunction, statement, textToLog) { gpk_logFunction(textToLog); statement; }	// Block with a statement preceded by a log call.
#ifdef GPK_WINDOWS
#	define block_logf(gpk_logFunction, statement, format, ...) { gpk_logFunction(format, __VA_ARGS__); statement; }	// Block with a statement preceded by a formatted log call.
#else
#	define block_logf(gpk_logFunction, statement, format, ...) { gpk_logFunction(format, ##__VA_ARGS__); statement; }	// Block with a statement preceded by a formatted log call.
#endif
// - Conditional log of condition.
#define if_true_log(gpk_logFunction, condition)											if(condition) { gpk_logFunction(#condition); }											// - Log condition if (condition) == true.
#define if_true_block_log(gpk_logFunction, condition, statement)						if(condition) block_log(gpk_logFunction, statement, #condition)							// - Conditional block with custom statement preceded by a log call.

// - Conditional log formatted.
#ifdef GPK_WINDOWS
#	define if_true_logf(gpk_logFunction, condition, format, ...)						if(condition) { gpk_logFunction(#condition "->" format, __VA_ARGS__); }					// - Log condition and custom formatted string args if (condition) == true.
#	define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)		if(condition) block_logf(gpk_logFunction, statement, #condition "->" format, __VA_ARGS__)	// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#else
#	ifdef GPK_ATMEL
#		define if_true_logf(gpk_logFunction, condition, format, ...)					if_true_log			(gpk_logFunction, condition)				// - Log condition and custom formatted string args if (condition) == true.
#		define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)	if_true_block_log	(gpk_logFunction, condition, statement) 	// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#	else
#		define if_true_logf(gpk_logFunction, condition, format, ...)					if(condition) { gpk_logFunction(#condition "->" format, ##__VA_ARGS__); }						// - Log condition and custom formatted string args if (condition) == true.
#		define if_true_block_logf(gpk_logFunction, condition, statement, format, ...)	if(condition) block_logf(gpk_logFunction, statement, #condition "->" format, ##__VA_ARGS__)		// - Conditional block with custom statement preceded by a log call with custom formatted string args.
#	endif
#endif

#define if_true_log_and_break(gpk_logFunction, condition)								if_true_block_log(gpk_logFunction, condition, break)					// - Log condition and break if (condition) == true.
#define if_true_log_and_continue(gpk_logFunction, condition)							if_true_block_log(gpk_logFunction, condition, continue)					// - Log condition and continue if (condition) == true.
#define if_true_log_and_throw(gpk_logFunction, condition)								if_true_block_log(gpk_logFunction, condition, gpk_throw(#condition))	// - Log condition and return if (condition) == true.
#define if_true_log_and_return(gpk_logFunction, condition)								if_true_block_log(gpk_logFunction, condition, return)					// - Log condition and return if (condition) == true.
#define if_true_log_and_return_value(gpk_logFunction, value, condition)					if_true_block_log(gpk_logFunction, condition, return (value))	// - Log condition and return some value if (condition) == true.
#define if_true_log_and_fail(gpk_logFunction, condition)								if_true_log_and_return_value(gpk_logFunction, -1, condition)			// - Log condition and return some value if (condition) == true.
#define if_true_log_and_return_zero(gpk_logFunction, condition)							if_true_log_and_return_value(gpk_logFunction, 0, condition)			// - Log condition and return some value if (condition) == true.
#define if_true_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_block_logf(gpk_logFunction, condition, break, format, __VA_ARGS__)			// - Log formatted string and break if (condition) == true.
#define if_true_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_block_logf(gpk_logFunction, condition, continue, format, __VA_ARGS__)			// - Log formatted string and continue if (condition) == true.
#define if_true_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_block_logf(gpk_logFunction, condition, gpk_throw(#condition), format, __VA_ARGS__)			// - Log condition and return if (condition) == true.
#define if_true_logf_and_return(gpk_logFunction, condition, format, ...)				if_true_block_logf(gpk_logFunction, condition, return, format, __VA_ARGS__)			// - Log formatted string and return if (condition) == true.
#define if_true_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_block_logf(gpk_logFunction, condition, return (value), format, __VA_ARGS__)	// - Log formatted string and return some value if (condition) == true.
#define if_true_logf_and_fail(gpk_logFunction, condition, format, ...)					if_true_logf_and_return_value(gpk_logFunction, -1, condition, format, __VA_ARGS__)	// - Log formatted string and return -1 if (condition) == true.
#define if_true_logf_and_return_zero(gpk_logFunction, condition, format, ...)			if_true_logf_and_return_value(gpk_logFunction, 0, condition, format, __VA_ARGS__)	// - Log formatted string and return -1 if (condition) == true.
//
#define if_fail_log(gpk_logFunction, condition)											if_true_log					(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> log
#define if_fail_log_and_break(gpk_logFunction, condition)								if_true_log_and_break		(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> break
#define if_fail_log_and_continue(gpk_logFunction, condition)							if_true_log_and_continue	(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> continue
#define if_fail_log_and_throw(gpk_logFunction, condition)								if_true_log_and_throw		(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> return
#define if_fail_log_and_return(gpk_logFunction, condition)								if_true_log_and_return		(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> return
#define if_fail_log_and_return_value(gpk_logFunction, value, condition)					if_true_log_and_return_value(gpk_logFunction, (value), ::gpk::failed(condition))	// - if condition < 0 -> return value
#define if_fail_log_and_fail(gpk_logFunction, condition)								if_true_log_and_fail		(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> return -1
#define if_fail_log_and_return_zero(gpk_logFunction, condition)							if_true_log_and_return_zero	(gpk_logFunction, ::gpk::failed(condition))				// - if condition < 0 -> return -1
#define if_zero_log(gpk_logFunction, condition)											if_true_log					(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> log
#define if_zero_log_and_break(gpk_logFunction, condition)								if_true_log_and_break		(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> break
#define if_zero_log_and_continue(gpk_logFunction, condition)							if_true_log_and_continue	(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> continue
#define if_zero_log_and_throw(gpk_logFunction, condition)								if_true_log_and_throw		(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> return
#define if_zero_log_and_return(gpk_logFunction, condition)								if_true_log_and_return		(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> return
#define if_zero_log_and_return_value(gpk_logFunction, value, condition)					if_true_log_and_return_value(gpk_logFunction, (value), 0 == (condition))			// - if condition == 0 -> return value
#define if_zero_log_and_fail(gpk_logFunction, condition)								if_true_log_and_fail		(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> return -1
#define if_zero_log_and_return_zero(gpk_logFunction, condition)							if_true_log_and_return_zero	(gpk_logFunction, 0 == (condition))						// - if condition == 0 -> return -1
#define if_fail_logf(gpk_logFunction, condition, format, ...)							if_true_logf					(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> log
#define if_fail_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_logf_and_break			(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> break
#define if_fail_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_logf_and_continue		(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> continue
#define if_fail_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_logf_and_throw			(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> return
#define if_fail_logf_and_return(gpk_logFunction, condition, format, ...)				if_true_logf_and_return			(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> return
#define if_fail_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_logf_and_return_value	(gpk_logFunction, (value), ::gpk::failed(condition), format, __VA_ARGS__)	// - if condition < 0 -> return value
#define if_fail_logf_and_fail(gpk_logFunction, condition, format, ...)					if_true_logf_and_fail			(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> return -1
#define if_fail_logf_and_return_zero(gpk_logFunction, condition, format, ...)			if_true_logf_and_return_zero	(gpk_logFunction, ::gpk::failed(condition), format, __VA_ARGS__)			// - if condition < 0 -> return -1
#define if_zero_logf(gpk_logFunction, condition, format, ...)							if_true_logf					(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> log
#define if_zero_logf_and_break(gpk_logFunction, condition, format, ...)					if_true_logf_and_break			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> break
#define if_zero_logf_and_continue(gpk_logFunction, condition, format, ...)				if_true_logf_and_continue		(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> continue
#define if_zero_logf_and_throw(gpk_logFunction, condition, format, ...)					if_true_logf_and_throw			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> return
#define if_zero_logf_and_return(gpk_logFunction, condition, format, ...)				if_true_logf_and_return			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> return
#define if_zero_logf_and_return_value(gpk_logFunction, value, condition, format, ...)	if_true_logf_and_return_value	(gpk_logFunction, (value), 0 == (condition), format, __VA_ARGS__)			// - if condition == 0 -> return value
#define if_zero_logf_and_fail(gpk_logFunction, condition, format, ...)					if_true_logf_and_fail			(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> return -1
#define if_zero_logf_and_return_zero(gpk_logFunction, condition, format, ...)			if_true_logf_and_return_zero	(gpk_logFunction, 0 == (condition), format, __VA_ARGS__)					// - if condition == 0 -> return -1
#define if_null_log						if_zero_log						// - if condition == 0 -> log
#define if_null_log_and_break			if_zero_log_and_break			// - if condition == 0 -> break
#define if_null_log_and_continue		if_zero_log_and_continue		// - if condition == 0 -> continue
#define if_null_log_and_throw			if_zero_log_and_throw			// - if condition == 0 -> return
#define if_null_log_and_return			if_zero_log_and_return			// - if condition == 0 -> return
#define if_null_log_and_return_value	if_zero_log_and_return_value	// - if condition == 0 -> return value
#define if_null_log_and_fail			if_zero_log_and_fail			// - if condition == 0 -> return -1
#define if_null_log_and_return_zero		if_zero_log_and_return_zero		// - if condition == 0 -> return 0
#define if_null_logf					if_zero_logf					// - if condition == 0 -> log
#define if_null_logf_and_break			if_zero_logf_and_break			// - if condition == 0 -> break
#define if_null_logf_and_continue		if_zero_logf_and_continue		// - if condition == 0 -> continue
#define if_null_logf_and_throw			if_zero_logf_and_throw			// - if condition == 0 -> return
#define if_null_logf_and_return			if_zero_logf_and_return			// - if condition == 0 -> return
#define if_null_logf_and_return_value	if_zero_logf_and_return_value	// - if condition == 0 -> return value
#define if_null_logf_and_fail			if_zero_logf_and_fail			// - if condition == 0 -> return -1
#define if_null_logf_and_return_zero	if_zero_logf_and_return_zero	// - if condition == 0 -> return 0

#endif // GPK_LOG_CORE_H
