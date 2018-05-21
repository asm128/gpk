#include "gpk_gui.h"

::gpk::error_t									gpk::guiControlCreate			(::gpk::SGUI& guiSystem, const ::gpk::SControlProperties& definition)						{
	::gpk::SRectangle2D<int32_t>						areaRealigned				= {};
	::gpk::realignRectangle(guiSystem.TargetSize, definition.Area, areaRealigned, definition.AlignArea);

	::gpk::SControlPropertyTable						& controlTable				= guiSystem.Properties[::gpk::CONTROL_STATE_Idle];
	int32_t												oldSize						= controlTable.AreaRealigned.size();
	int32_t												resultCheck	[]				= 
		//{	controlTable.AreaRealigned			.push_back(areaRealigned			)
		{	controlTable.Text					.push_back(definition.Text			)
		,	controlTable.Area					.push_back(definition.Area			)
		,	controlTable.AreaRealigned			.push_back(areaRealigned			) //definition.AreaRealigned	)
		,	controlTable.ColorSurface			.push_back(definition.ColorSurface	)
		,	controlTable.ColorText				.push_back(definition.ColorText		)
		,	controlTable.SizeBorder				.push_back(definition.SizeBorder	)
		,	controlTable.SizeMargin				.push_back(definition.SizeMargin	)
		,	controlTable.AlignArea				.push_back(definition.AlignArea		)
		,	controlTable.AlignText				.push_back(definition.AlignText		)
		};

	for(uint32_t iState = 1; iState < CONTROL_STATE_COUNT; ++iState) {
		//guiSystem.Properties[iState].AreaRealigned	.push_back(definition.AreaRealigned	);
		guiSystem.Properties[iState].Text			.push_back(definition.Text			);
		guiSystem.Properties[iState].Area			.push_back(definition.Area			);
		guiSystem.Properties[iState].AreaRealigned	.push_back(areaRealigned			); //definition.AreaRealigned	);
		guiSystem.Properties[iState].ColorSurface	.push_back(definition.ColorSurface	);
		guiSystem.Properties[iState].ColorText		.push_back(definition.ColorText		);
		guiSystem.Properties[iState].SizeBorder		.push_back(definition.SizeBorder	);
		guiSystem.Properties[iState].SizeMargin		.push_back(definition.SizeMargin	);
		guiSystem.Properties[iState].AlignArea		.push_back(definition.AlignArea		);
		guiSystem.Properties[iState].AlignText		.push_back(definition.AlignText		);
	}

	for(uint32_t iCheck = 0, checkCount = ::gpk::size(resultCheck) - 1; iCheck < checkCount; ++iCheck )
		if(resultCheck[iCheck] != resultCheck[iCheck + 1]) {
			error_printf("Control property indices don't match. The reasons for this to happen are running out of memory or mismanagement of the control table.");
			controlTable.Resize(oldSize);
			return -1;
		}
	return 0;
}


static						::gpk::error_t		updateGUIControlHovered			(::gpk::SControlState& controlFlags, const ::gpk::SInput& inputSystem)	noexcept{ 
	if(controlFlags.Hover) {
		if(inputSystem.ButtonDown(0) && false == controlFlags.Pressed) 
			controlFlags.Pressed							= true;
		else if(inputSystem.ButtonUp(0) && controlFlags.Pressed) {
			controlFlags.Execute							= true;
			controlFlags.Pressed							= false;
		}
	}
	else 
		controlFlags.Hover								= true;
	return 0;
}

::gpk::error_t									gpk::guiUpdate					(::gpk::SGUI& GUISystem, const ::gpk::SInput& inputSystem)										{ 
	const ::gpk::SCoord2<int32_t>						& mousePos						= GUISystem.MousePosition = inputSystem.MouseCurrent.Position;
	::gpk::SControlPropertyTable						& controlsIdle					= GUISystem.Properties[::gpk::CONTROL_STATE_Idle];
	::gpk::array_pod<::gpk::SControlState>				& controlsState					= GUISystem.Controls;
	for(uint32_t iControl=0; iControl < controlsState.size(); ++iControl) {
		::gpk::SRectangle2D<int32_t>						& controlArea					= controlsIdle.AreaRealigned	[iControl];
		::gpk::SControlState								& controlFlags					= controlsState[iControl];
		
		// EXECUTE only lasts one tick.
		if (controlFlags.Execute)
			controlFlags.Execute							= false;

		//--------------------
		if(::gpk::in_range(mousePos, controlArea))
			updateGUIControlHovered(controlFlags, inputSystem);
		else {
			if (controlFlags.Hover)
				controlFlags.Hover									= false;

			if(inputSystem.ButtonUp(0) && controlFlags.Pressed)
				controlFlags.Pressed								= false;
		}
	}
	return 0;
}
