#include "gpk_array.h"
#include <windows.h>

static	const char				test_chars[]		= {'F', 'r', 'i', 'e', 'n', 'd'};

int test_array_pod() {
	static	const ::gpk::apod<char>	aconst_0			= ::gpk::vcs{"Hello"};
	static	const ::gpk::apod<char>	aconst_1			{test_chars};
	::gpk::apod<char>			atest_0				= "Again ";
	::gpk::apod<char>			atest_1				= {'L', 'a', 't', 'e', 'r'};
	OutputDebugString("\n\n");

	always_printf("test_chars: %u, %u, %s", (uint32_t)strlen(test_chars), (uint32_t)::gpk::size(test_chars), test_chars);
	always_printf("aconst_0  : %u, %s", aconst_0	.size(), aconst_0	.begin());
	always_printf("aconst_1  : %u, %s", aconst_1	.size(), aconst_1	.begin());
	always_printf("atest_0   : %u, %s", atest_0		.size(), atest_0	.begin());
	always_printf("atest_1   : %u, %s", atest_1		.size(), atest_1	.begin());
	OutputDebugString("\n\n");

	::gpk::view<const char>			vtest_0(aconst_0), vtest_1{atest_1};
	atest_0						= vtest_0;
	atest_1						= aconst_1;
	always_printf("atest_0   : %u, %s", atest_0		.size(), atest_0	.begin());
	always_printf("atest_1   : %u, %s", atest_1		.size(), atest_1	.begin());
	OutputDebugString("\n\n");

	char							popped				= 0;									    
	gpk_necs(atest_0.pop_back		());						always_printf("pop_back             : %u, %s"		, atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.pop_back		(popped));					always_printf("pop_back             : %u, %s, '%c'"	, atest_1.size(), atest_1.begin(), popped);
	gpk_necs(atest_1.push_back		(popped = '$'));			always_printf("push_back            : %u, %s"		, atest_1.size(), atest_1.begin());
	gpk_necs(atest_0.append_string	(" again"));				always_printf("append_string        : %u, %s"		, atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.append_string	(::gpk::vcs{" value"}));	always_printf("append_string        : %u, %s"		, atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0.clear			();			always_printf("clear                : %u, 0x%p", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer	();			always_printf("clear_pointer        : %u, 0x%p", atest_1.size(), atest_1.begin());
	atest_0.resize			(6);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(6, 'K');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.resize			(64);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(64, 'D');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.resize			(4);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(4, 'K');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0						= aconst_0;	always_printf("atest_0              : %u, %s", atest_0.size(), atest_0.begin());
	atest_1						= aconst_1;	always_printf("atest_1              : %u, %s", atest_1.size(), atest_1.begin());

	atest_1 = aconst_1;	gpk_necs(atest_1.insert(0, '-'));						always_printf("insert at 0          : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(0, ::gpk::vcs{"Testing "}));	always_printf("insert at 0          : %u, %s", atest_0.size(), atest_0.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(0, test_chars));				always_printf("insert at 0          : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.insert(0, "Bye ", 3));					always_printf("insert at 0          : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1 = aconst_1;	gpk_necs(atest_1.insert(1, '!'));						always_printf("insert at 1          : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(1, ::gpk::vcs{"Testing "}));	always_printf("insert at 1          : %u, %s", atest_0.size(), atest_0.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(1, test_chars));				always_printf("insert at 1          : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.insert(1, "Amigo ", 5));				always_printf("insert at 1          : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size() - 1, '?'));					always_printf("insert at .size() - 1: %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size() - 1, ::gpk::vcs{" Luke "}));	always_printf("insert at .size() - 1: %u, %s", atest_0.size(), atest_0.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size() - 1, test_chars));			always_printf("insert at .size() - 1: %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size() - 1, " Chao ", 6));			always_printf("insert at .size() - 1: %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size(), '*'));					always_printf("insert at .size()    : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size(), ::gpk::vcs{" Verde"}));	always_printf("insert at .size()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size(), test_chars));			always_printf("insert at .size()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size(), " The ", 5));			always_printf("insert at .size()    : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	gpk_necs(atest_0.resize(64));		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.resize(64, 'K'));	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	gpk_necs(atest_0.resize(4));		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.resize(4, 'K'));	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0	= aconst_0;	always_printf("atest_0     : %u, %s", atest_0.size(), atest_0.begin());
	atest_1	= aconst_1;	always_printf("atest_1     : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.append(::gpk::vcs{" Verde"}));		always_printf("append()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.append(test_chars));				always_printf("append()    : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0 = aconst_0;	gpk_necs(atest_0.remove				(3));					always_printf("remove          (3)        : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(3));					always_printf("remove_unordered(3)        : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.remove				(0));					always_printf("remove          (0)        : %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(0));					always_printf("remove_unordered(0)        : %u, %s", atest_1.size(), atest_1.begin());
	atest_0 = aconst_0;	gpk_necs(atest_0.remove				(atest_0.size()-1));	always_printf("remove          (.size()-1): %u, %s", atest_0.size(), atest_0.begin());
	atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(atest_1.size()-1));	always_printf("remove_unordered(.size()-1): %u, %s", atest_1.size(), atest_1.begin());

	// erase
	// find

	return 0;
}

int test_array_obj() {
	const ::gpk::aobj<char>			aconst_0			= ::gpk::vcs{"Hello"};
	const ::gpk::aobj<char>			aconst_1			{test_chars};
	::gpk::aobj<char>				atest_0				= "Again ";
	::gpk::aobj<char>				atest_1				= {'L', 'a', 't', 'e', 'r'};
	OutputDebugString("\n\n");

	always_printf("test_chars: %u, %u, %s", (uint32_t)strlen(test_chars), (uint32_t)::gpk::size(test_chars), test_chars);
	always_printf("aconst_0  : %u, %s", aconst_0	.size(), aconst_0	.begin());
	always_printf("aconst_1  : %u, %s", aconst_1	.size(), aconst_1	.begin());
	always_printf("atest_0   : %u, %s", atest_0		.size(), atest_0	.begin());
	always_printf("atest_1   : %u, %s", atest_1		.size(), atest_1	.begin());
	OutputDebugString("\n\n");

	::gpk::view<const char>			vtest_0(aconst_0), vtest_1{atest_1};
	atest_1						= aconst_1;
	atest_0						= vtest_0;
	always_printf("atest_0   : %u, %s", atest_0		.size(), atest_0	.begin());
	always_printf("atest_1   : %u, %s", atest_1		.size(), atest_1	.begin());
	OutputDebugString("\n\n");

	char							popped				= 0;									    
	gpk_necs(atest_0.pop_back		(popped));					always_printf("pop_back             : %u, %s", atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.pop_back		());						always_printf("pop_back             : %u, %s", atest_1.size(), atest_1.begin());
	gpk_necs(atest_0.push_back		(popped));					always_printf("push_back            : %u, %s", atest_0.size(), atest_0.begin());
	OutputDebugString("\n\n");

	atest_0.clear			();			always_printf("clear                : %u, 0x%p", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer	();			always_printf("clear_pointer        : %u, 0x%p", atest_1.size(), atest_1.begin());
	atest_0.resize			(6);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(6, 'K');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.resize			(64);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(64, 'D');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.resize			(4);		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.resize			(4, 'K');	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0						= aconst_0;	always_printf("atest_0              : %u, %s", atest_0.size(), atest_0.begin());
	atest_1						= aconst_1;	always_printf("atest_1              : %u, %s", atest_1.size(), atest_1.begin());

	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(0, '-'));						always_printf("insert at 0          : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(0, ::gpk::vcs{"Testing "}));	always_printf("insert at 0          : %u, %s", atest_0.size(), atest_0.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(0, test_chars));				always_printf("insert at 0          : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(0, "Bye ", 3));					always_printf("insert at 0          : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(1, '!'));						always_printf("insert at 1          : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(1, ::gpk::vcs{"Testing "}));	always_printf("insert at 1          : %u, %s", atest_0.size(), atest_0.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(1, test_chars));				always_printf("insert at 1          : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(1, "Amigo ", 5));				always_printf("insert at 1          : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size() - 1, '?'));					always_printf("insert at .size() - 1: %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size() - 1, ::gpk::vcs{" Luke "}));	always_printf("insert at .size() - 1: %u, %s", atest_0.size(), atest_0.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size() - 1, test_chars));			always_printf("insert at .size() - 1: %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size() - 1, " Chao ", 6));			always_printf("insert at .size() - 1: %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size(), '*'));					always_printf("insert at .size()    : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size(), ::gpk::vcs{" Verde"}));	always_printf("insert at .size()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.insert(atest_0.size(), test_chars));			always_printf("insert at .size()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.insert(atest_1.size(), " The ", 5));			always_printf("insert at .size()    : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	gpk_necs(atest_0.resize(64));		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.resize(64, 'K'));	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	gpk_necs(atest_0.resize(4));		always_printf("resize               : %u, %s", atest_0.size(), atest_0.begin());
	gpk_necs(atest_1.resize(4, 'K'));	always_printf("resize               : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0.clear_pointer(); atest_0	= aconst_0;	always_printf("atest_0     : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1	= aconst_1;	always_printf("atest_1     : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.append(::gpk::vcs{" Verde"}));		always_printf("append()    : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.append(test_chars));				always_printf("append()    : %u, %s", atest_1.size(), atest_1.begin());
	OutputDebugString("\n\n");

	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.remove				(3));					always_printf("remove          (3)        : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(3));					always_printf("remove_unordered(3)        : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.remove				(0));					always_printf("remove          (0)        : %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(0));					always_printf("remove_unordered(0)        : %u, %s", atest_1.size(), atest_1.begin());
	atest_0.clear_pointer(); atest_0 = aconst_0;	gpk_necs(atest_0.remove				(atest_0.size()-1));	always_printf("remove          (.size()-1): %u, %s", atest_0.size(), atest_0.begin());
	atest_1.clear_pointer(); atest_1 = aconst_1;	gpk_necs(atest_1.remove_unordered	(atest_1.size()-1));	always_printf("remove_unordered(.size()-1): %u, %s", atest_1.size(), atest_1.begin());

	// erase
	// find

	return 0;
}

int main() {
	gpk_necs(test_array_pod());
	gpk_necs(test_array_obj());
	return EXIT_SUCCESS;
}
