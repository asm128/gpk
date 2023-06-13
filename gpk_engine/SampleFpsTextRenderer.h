﻿#include "DeviceResources.h"

#include "gpk_ptr.h"

#include <string>

#ifndef SAMPLEFPSTEXTRENDERER_H_9238679823
#define SAMPLEFPSTEXTRENDERER_H_9238679823

namespace gpk
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	struct SampleFpsTextRenderer {
		// Cached pointer to device resources.
		::gpk::pobj<::DX::D3DDeviceResources>	DeviceResources					= {};

		// Resources related to text rendering.
		std::wstring							Text							= {};
		DWRITE_TEXT_METRICS						TextMetrics						= {};
		::gpk::pcom<ID2D1SolidColorBrush>		WhiteBrush						= {};
		::gpk::pcom<ID2D1DrawingStateBlock1>	StateBlock						= {};
		::gpk::pcom<IDWriteTextLayout3>			TextLayout						= {};
		::gpk::pcom<IDWriteTextFormat2>			TextFormat						= {};

		::gpk::error_t							CreateDeviceResources	(ID2D1DeviceContext * d2dContext) { WhiteBrush = {}; gpk_hrcall(d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &WhiteBrush)); return 0; }
		void									ReleaseDeviceResources	() { WhiteBrush = {}; }

		// Initializes D2D resources used for text 
		::gpk::error_t							Initialize		(const ::gpk::pobj<DX::D3DDeviceResources> & deviceResources)  {
			DeviceResources									= deviceResources;
			// Create device independent resources
			::gpk::pcom<IDWriteTextFormat>						textFormat						= {};
			gpk_hrcall(DeviceResources->GetDWriteFactory()->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", &textFormat));
			gpk_necs(textFormat.as(TextFormat));
			gpk_hrcall(TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
			gpk_hrcall(DeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&StateBlock));
			return CreateDeviceResources(DeviceResources->GetD2DDeviceContext());
		}

		// Updates the text to be displayed.
		::gpk::error_t									Update							(double /*frameSeconds*/, double /*totalSeconds*/, uint32_t framesPerSecond) {
			// Update display text.
			uint32_t											fps								= framesPerSecond;
			Text											= (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

			::gpk::pcom<IDWriteTextLayout>						textLayout;
			constexpr float										w								= 240.0f; // Max width of the input text.
			constexpr float										h								= 50.0f;// Max height of the input text.
			gpk_hrcall(DeviceResources->GetDWriteFactory()->CreateTextLayout(Text.c_str(), (uint32_t) Text.length(), TextFormat, w, h, &textLayout));
			gpk_necs(textLayout.as(TextLayout));
			gpk_hrcall(TextLayout->GetMetrics(&TextMetrics));
			return 0;
		}

		// Renders a frame to the screen.
		::gpk::error_t									Render							() {
			ID2D1DeviceContext									* context						= DeviceResources->GetD2DDeviceContext();
			const ::gpk::n2f32								logicalSize						= DeviceResources->GetLogicalSize();

			context->SaveDrawingState(StateBlock);
			context->BeginDraw();

			// Position on the bottom right corner
			D2D1::Matrix3x2F									screenTranslation				= D2D1::Matrix3x2F::Translation(0, logicalSize.y - TextMetrics.height);
			context->SetTransform(screenTranslation * DeviceResources->GetOrientationTransform2D());
			gpk_hrcall(TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));

			context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), TextLayout, WhiteBrush);
	
			HRESULT												hr								= context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device is lost. It will be handled during the next call to Present.
				gpk_hrcall(hr);

			context->RestoreDrawingState(StateBlock);
			return 0;
		}

	};
}

#endif // SAMPLEFPSTEXTRENDERER_H_9238679823