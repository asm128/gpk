#include "gpk_array.h"
#include "gpk_color.h"
#include "gpk_input.h"
#include <string>

#ifndef GPK_GUI_H_2903874982374
#define GPK_GUI_H_2903874982374

namespace gpk
{
#pragma pack(push, 1)
	enum CONTROL_STATE	: int8_t
		{ CONTROL_STATE_Idle
		, CONTROL_STATE_Hover		
		, CONTROL_STATE_Pressed		
		, CONTROL_STATE_Selected	
		, CONTROL_STATE_Execute
		, CONTROL_STATE_Disabled	
		, CONTROL_STATE_Unused		
		, CONTROL_STATE_COUNT		
		, CONTROL_STATE_INVALID			= (int8_t)-1
		};

	struct SControlState {
							bool												Hover									: 1;
							bool												Pressed									: 1;
							bool												Selected								: 1;
							bool												Execute									: 1;
							bool												Disabled								: 1;
							bool												Unused									: 1;
	};

	struct SControlPropertyIndices {
							int16_t												Text									= -1;
							int16_t												Area									= -1;
							int16_t												AreaRealigned							= -1;
							int16_t												Center									= -1;
							int16_t												ColorFill								= -1;
							int16_t												ColorBorder								= -1;
							int16_t												SizeBorder								= -1;
							int16_t												SizeMargin								= -1;
							int16_t												SizePadding								= -1;
	};

	struct SGUIColorSurface {
							::gpk::SColorBGRA									Fill									= {};
							::gpk::SColorBGRA									Border									= {};
	};

	struct SGUIColorText {
							::gpk::SColorBGRA									Fill									= {};
							::gpk::SColorBGRA									Border									= {};
							::gpk::SColorBGRA									Background								= {};
	};
#pragma pack(pop)

	struct SControlProperties {
							::std::string										Text									= {};
							::std::string										Tooltip									= {};
							::gpk::SRectangle2D<int32_t>						Area									= {};
							::gpk::SRectangle2D<int32_t>						AreaControl								= {};
							::gpk::SRectangle2D<int32_t>						AreaText								= {};
							::gpk::SRectangle2D<int32_t>						AreaRealigned							= {};
							::gpk::SCoord2<int32_t>								Center									= {};
							::gpk::SGUIColorSurface								ColorSurface							= {};
							::gpk::SGUIColorText								ColorText								= {};
							::gpk::SCoord2<uint32_t>							SizeBorder								= {};
							::gpk::SCoord2<uint32_t>							SizeMargin								= {};
							::gpk::ALIGN										AlignArea								= {};
							::gpk::ALIGN										AlignText								= {};
	};

	struct SControlPropertyTable {
							::gpk::array_pod<::std::string					>	Text									= {};
							::gpk::array_pod<::std::string					>	Tooltip									= {};
							::gpk::array_pod<::gpk::SRectangle2D<int32_t>	>	Area									= {};
							::gpk::array_pod<::gpk::SRectangle2D<int32_t>	>	AreaRealigned							= {};
							::gpk::array_pod<::gpk::SCoord2<int32_t>		>	Center									= {};
							::gpk::array_pod<::gpk::SGUIColorSurface		>	ColorSurface							= {};
							::gpk::array_pod<::gpk::SGUIColorText			>	ColorText								= {};
							::gpk::array_pod<::gpk::SCoord2<uint32_t>		>	SizeBorder								= {};
							::gpk::array_pod<::gpk::SCoord2<uint32_t>		>	SizeMargin								= {};
							::gpk::array_pod<::gpk::ALIGN					>	AlignArea								= {};
							::gpk::array_pod<::gpk::ALIGN					>	AlignText								= {};

							// -------------
							void												Clear									()						{
								::gpk::clear
									(	Text				
									,	Area				
									,	AreaRealigned		
									,	ColorSurface		
									,	ColorText			
									,	SizeBorder			
									,	SizeMargin			
									,	AlignArea			
									,	AlignText			
									);
							}

							void												Resize									(uint32_t newSize)		{
								::gpk::resize
									(	newSize
									,	Text				
									,	Area				
									,	AreaRealigned		
									,	ColorSurface		
									,	ColorText			
									,	SizeBorder			
									,	SizeMargin			
									,	AlignArea			
									,	AlignText			
									);
							}

	};

	struct SGUI {	// 
							::gpk::SControlPropertyTable						Properties		[CONTROL_STATE_COUNT]	= {};
							::gpk::array_pod<::gpk::SControlState>				Controls								= {};
							::gpk::SCoord2<uint32_t>							TargetSize								= {};
							::gpk::SCoord2<int32_t>								MousePosition							= {};
	};

						::gpk::error_t										guiControlCreate						(::gpk::SGUI& guiSystem, const ::gpk::SControlProperties& controlProperties);
						::gpk::error_t										guiUpdate								(::gpk::SGUI& GUISystem, const ::gpk::SInput& inputSystem);
} // namespace

#endif // GPK_GUI_H_2903874982374

