#ifndef GPK_DIALOG_H_67383889976
#define GPK_DIALOG_H_67383889976

namespace gpk

{
	struct IControl {
	    virtual	IControl() {};
        virtual		::gpk::error_t Update();
	};

	struct SDialogManager {
		::gpk::array_obj<::gpk::ptr_obj<IControl>>  Controls;
		
	};
}

#endif // GPK_DIALOG_H_67383889976