#include "gpk_label.h"
#include "gpk_label_manager.h"

static	::gpk::CLabelManager*	labelManagerSingleton		()			noexcept	{ 
	static	::gpk::CLabelManager	instanceLabelManager; 
	return &instanceLabelManager; 
}

		::gpk::label::label			(const char * text, uint32_t stringLen)	noexcept	: LabelManager(::labelManagerSingleton())	{ 
	e_if_failed(LabelManager->View(text, stringLen, *this), "text: %s.", text); 
}

bool	::gpk::label::operator==	(const ::gpk::vcs & other)		const	noexcept	{
	return (Data == other.begin()) 	? true
		: (Count != other.size()) 	? false 
		: 0 == memcmp(Data, other.begin(), Count)
		;
}

bool	::gpk::label::operator==	(const label & other)			const	noexcept	{
	return (Data == other.Data) 	? true
		: (Count != other.Count) 	? false
		: (LabelManager == other.LabelManager) ? false 
		: 0 == memcmp(Data, other.Data, Count)
		;
}
