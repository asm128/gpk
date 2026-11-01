#include "llc_view_color.h"

::llc::err_t llc::vcu82bgr8(vcu0_t bytes, ::llc::bgr8 & colorToSet) {
	if_true_fe(szof(::llc::bgr8) > bytes.size());
	colorToSet.r = bytes[2]; 
	colorToSet.g = bytes[1]; 
	colorToSet.b = bytes[0]; 
	return 3;
}

::llc::err_t llc::vcu82bgra8(vcu0_t bytes, ::llc::bgra8 & colorToSet) {
	if_true_fe(szof(::llc::bgra8) > bytes.size());
	colorToSet.a = bytes[3];
	colorToSet.r = bytes[2]; 
	colorToSet.g = bytes[1]; 
	colorToSet.b = bytes[0]; 
	return 4;
	//u0_t readBytes = 0;
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
