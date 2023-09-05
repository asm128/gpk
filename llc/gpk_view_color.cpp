#include "gpk_view_color.h"

::gpk::error_t gpk::vcu82bgr8(const ::gpk::vcu8 & bytes, ::gpk::bgr8 & colorToSet) {
	rees_if(sizeof(::gpk::bgr8) > bytes.size());
	colorToSet.r = bytes[2]; 
	colorToSet.g = bytes[1]; 
	colorToSet.b = bytes[0]; 
	return 3;
}

::gpk::error_t gpk::vcu82bgra8(const ::gpk::vcu8 & bytes, ::gpk::bgra8 & colorToSet) {
	rees_if(sizeof(::gpk::bgra8) > bytes.size());
	colorToSet.a = bytes[3];
	colorToSet.r = bytes[2]; 
	colorToSet.g = bytes[1]; 
	colorToSet.b = bytes[0]; 
	return 4;
	//uint8_t readBytes = 0;
	//switch(bytes.size()) {
	//case 4: ++readBytes; colorToSet.a = bytes[3];
	//case 3: ++readBytes; colorToSet.r = bytes[2]; if(bytes.size() == 3) colorToSet.a	= 0xFF;
	//case 2: ++readBytes; colorToSet.g = bytes[1]; if(bytes.size() == 2) colorToSet.r	= 0;
	//case 1: ++readBytes; colorToSet.b = bytes[0]; if(bytes.size() == 1) colorToSet.g	= 0;
	//case 0: 
	//	return -1;
	//}
	//return readBytes;
}