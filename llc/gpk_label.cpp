#include "gpk_label.h"
#include "gpk_label_manager.h"

sttc	gpk::CLabelManager*	labelManagerSingleton	()	nxpt	{ 
	sttc	::gpk::CLabelManager	instanceLabelManager; 
	return &instanceLabelManager; 
}
		gpk::label::label	(sc_c * text, u2_t stringLen)	nxpt
	: LabelManager(::labelManagerSingleton()) {
	if_fail_ef(LabelManager->View(text, stringLen, *this), "text: %.*s.", (int)stringLen, text);
}
bool	gpk::label::oper==	(vcst_t other)			csnx	{
	return (Data == other.begin()) 	? true
		: (Count != other.size()) 	? false 
		: 0 == memcmp(Data, other.begin(), Count)
		;
}
bool	gpk::label::oper==	(cnst label & other)	csnx	{
	return (Data == other.Data) 	? true
		: (Count != other.Count) 	? false
		: (LabelManager == other.LabelManager) ? false 
		: 0 == memcmp(Data, other.Data, Count)
		;
}


