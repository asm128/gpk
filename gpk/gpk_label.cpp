#include "gpk_label.h"
#include "gpk_label_manager.h"

static	::gpk::CLabelManager*				labelManagerSingleton						()													noexcept	{ static ::gpk::CLabelManager instanceLabelManager; return &instanceLabelManager; }

											::gpk::label::label							(const char* text, uint32_t stringLen)				noexcept	: LabelManager(::labelManagerSingleton())			{ LabelManager->ArrayView(text, stringLen, *this); }

		bool								::gpk::label::operator==					(const label& other)						const	noexcept	{
	if(Data == other.Data)
		return true;
	if(Count != other.Count)
		return false;
	return (LabelManager == other.LabelManager) ? false : 0 == memcmp(Data, other.Data, Count);
}

		bool								::gpk::label::operator==					(const ::gpk::view_const_string& other)		const	noexcept	{
	if(Data == other.begin())
		return true;
	return (Count != other.size()) ? false : 0 == memcmp(Data, other.begin(), Count);
}
