#include "gpk_label.h"
#include "gpk_label_manager.h"

static	::gpk::CLabelManager*	labelManagerSingleton		()			nxpt	{ 
	static	::gpk::CLabelManager	instanceLabelManager; 
	return &instanceLabelManager; 
}

		::gpk::label::label			(vcst_c & text) : vcst_t(text), LabelManager(::labelManagerSingleton()) { 
	if_fail_tef(LabelManager->View(*this), "text: %.*s.", GPK_VIEW_ARGS(text)); 
}

bool	::gpk::label::oper==	(const label & other)			csnx	{
	return (Data == other.Data) 	? true
		: (Count != other.Count) 	? false
		: (LabelManager == other.LabelManager) ? false 
		: 0 == memcmp(Data, other.Data, Count)
		;
}
