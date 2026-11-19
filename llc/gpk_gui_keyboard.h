#include "gpk_array_static.h"
#include "gpk_ctti_enum.h"

#ifndef GPK_GUI_KEYBOARD_H_23627
#define GPK_GUI_KEYBOARD_H_23627

nmsp gpk
{
	stxp u0_t			GUI_KEYBOARD_Default_row_width	= 32;
	stxp cnst astu0_t<256>	GUI_KEYBOARD_Default_keys	=
		{   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16
		,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32
		,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48
		,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64
		,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80
		,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96
		,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112
		, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128
		, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144
		, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160
		, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176
		, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192
		, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208
		, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224
		, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240
		, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, '!', '?'
		};
	stxp cnst astu2_t<128>	GUI_KEYBOARD_CP437_UNICODE_HIGH =
		{ 0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7
		, 0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5
		, 0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9
		, 0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192
		, 0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA
		, 0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB
		, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556
		, 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510
		, 0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F
		, 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567
		, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B
		, 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580
		, 0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4
		, 0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229
		, 0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248
		, 0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
		};
	stin err_t		unicodeToCP437			(u2_t codePoint, u0_t & output) {
		if(codePoint < 0x80) {
			output = (u0_t)codePoint;
			return 0;
		}
		for(u2_t iCode = 0; iCode < GUI_KEYBOARD_CP437_UNICODE_HIGH.size(); ++iCode)
			if(GUI_KEYBOARD_CP437_UNICODE_HIGH[iCode] == codePoint) {
				output = (u0_t)(0x80 + iCode);
				return 0;
			}
		return -1;
	}
	CTTI_ENUM(VK_EVENT,
	enum VK_EVENT : u0_t
		{ VK_EVENT_Press	
		, VK_EVENT_Release	
		, VK_EVENT_Hold		
		, VK_EVENT_Edit		
		, VK_EVENT_Arrow
		};
	);
	CTTI_ENUM(VK_SCANCODE,
	enum VK_SCANCODE : u0_t
		{ VK_SCANCODE_Escape
		, VK_SCANCODE_Enter     
		, VK_SCANCODE_Backspace 
		, VK_SCANCODE_Clear     
		, VK_SCANCODE_Left      
		, VK_SCANCODE_Right     
		, VK_SCANCODE_Up        
		, VK_SCANCODE_Down      
		, VK_SCANCODE_Front     
		, VK_SCANCODE_Back      
		, VK_SCANCODE_Delete    
		, VK_SCANCODE_Insert
		, VK_SCANCODE_Home
		, VK_SCANCODE_End
		, VK_SCANCODE_Page_Up
		, VK_SCANCODE_Page_Down
		, VK_SCANCODE_LeftShift
		, VK_SCANCODE_RightShift
		, VK_SCANCODE_Alt
		, VK_SCANCODE_Alt_Gr
		, VK_SCANCODE_LControl
		, VK_SCANCODE_RControl
		, VK_SCANCODE_System // "Windows Key" on windows
		};
	);
#pragma pack(push, 1)
	struct SGUIKeyboardEvent {
		VK_EVENT		Type;
		VK_SCANCODE		ScanCode;
	};
#pragma pack(pop)
}

#endif // GPK_GUI_KEYBOARD_H_23627
