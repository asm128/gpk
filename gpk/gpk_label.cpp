#include "gpk_label.h"
#include "gpk_label_manager.h"

static	::gpk::CLabelManager*				labelManagerSingleton					()													noexcept	{
	static	::gpk::CLabelManager					instanceLabelManager;
	return &instanceLabelManager;
}

											::gpk::label::label						(const char* text, uint32_t stringLen)				noexcept	: LabelManager(labelManagerSingleton())
{
	LabelManager->ArrayView(text, stringLen, *this);	
}

				bool						gpk::label::operator==					(const label& other)						const	noexcept	{
	if(Data == other.Data)
		return true;
	if(Count != other.Count)
		return false;
	if(LabelManager == other.LabelManager)
		return false;
	return 0 == memcmp(Data, other.Data, Count);
}
