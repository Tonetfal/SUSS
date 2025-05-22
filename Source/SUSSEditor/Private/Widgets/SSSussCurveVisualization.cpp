// Author: Tom Werner (MajorT), 2025


#include "SSSussCurveVisualization.h"

#include "SCurveEditor.h"
#include "SlateOptMacros.h"
#include "SussUtility.h"

#define LOCTEXT_NAMESPACE "SussCurveVisualization"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSSussCurveVisualization::Construct(const FArguments& InArgs)
{
	DesiredSize = InArgs._DesiredSize;
	OnGetConsideration = InArgs._OnGetConsideration;

	ViewMinInput = InArgs._ViewMinInput;
	ViewMaxInput = InArgs._ViewMaxInput;
	ViewMinOutput = InArgs._ViewMinOutput;
	ViewMaxOutput = InArgs._ViewMaxOutput;
	DefaultMinInput = ViewMinInput.Get();
	DefaultMaxInput = ViewMaxInput.Get();
	DefaultMinOutput = ViewMinOutput.Get();
	DefaultMaxOutput = ViewMaxOutput.Get();

	//	UE_LOG(LogTemp, Error, TEXT("CurveType: %s - CurveParams: %s"), *UEnum::GetValueAsString(CurveType), *CurveParams.ToString());

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(32.f, 0.f, 0.f, 0.f)
			[
				SNew(SBorder)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Left)
				.BorderImage(FAppStyle::GetBrush("NoBorder"))
				.DesiredSizeScale(FVector2D(256.0f, 32.0f))
				.Padding(FMargin(2, 12, 0, 0))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ToolTipText(LOCTEXT("ZoomToFitHorizontal", "Zoom To Fit Horizontal"))
						.OnClicked(this, &SSSussCurveVisualization::ZoomToFitHorizontalClicked)
						.ContentPadding(1)
						[
							SNew(SImage)
							.Image(FAppStyle::GetBrush("CurveEd.FitHorizontal"))
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ToolTipText(LOCTEXT("ZoomToFitVertical", "Zoom To Fit Vertical"))
						.OnClicked(this, &SSSussCurveVisualization::ZoomToFitVerticalClicked)
						.ContentPadding(1)
						[
							SNew(SImage)
							.Image(FAppStyle::GetBrush("CurveEd.FitVertical"))
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
				]
			]
		]
	];
}

FVector2D SSSussCurveVisualization::ComputeDesiredSize(float X) const
{
	return DesiredSize.Get();
}

int32 SSSussCurveVisualization::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const FSlateBrush* TimelineAreaBrush = FAppStyle::GetBrush("CurveEd.TimelineArea");
	const FSlateBrush* WhiteBrush = FAppStyle::GetBrush("WhiteTexture");
	FGeometry CurveAreaGeometry = AllottedGeometry;
	FTrackScaleInfo ScaleInfo(ViewMinInput.Get(), ViewMaxInput.Get(), ViewMinOutput.Get(), ViewMaxOutput.Get(),
	                          CurveAreaGeometry.GetLocalSize());

	const int32 GridLayerId = LayerId;
	PaintGridLines(CurveAreaGeometry, ScaleInfo, OutDrawElements, GridLayerId, MyCullingRect, DrawEffects);

	const int32 CurveLayerId = GridLayerId + 1;
	PaintCurve(CurveAreaGeometry, ScaleInfo,
	           OutDrawElements, CurveLayerId, MyCullingRect, DrawEffects, InWidgetStyle, false);

	const int32 OtherLayerId = CurveLayerId + 1;
	return SCompoundWidget::OnPaint(Args, CurveAreaGeometry, MyCullingRect, OutDrawElements, OtherLayerId,
	                                InWidgetStyle, bParentEnabled);
}

float SSSussCurveVisualization::GetTimeStep(FTrackScaleInfo& ScaleInfo) const
{
	constexpr float MaxGridPixelSpacing = 150.0f;
	const float GridPixelSpacing = FMath::Min(ScaleInfo.WidgetSize.GetMin() / 1.5f, MaxGridPixelSpacing);
	const double MaxTimeStep = ScaleInfo.LocalXToInput(GridPixelSpacing) - ScaleInfo.LocalXToInput(0);

	return static_cast<float>(static_cast<double>(FMath::RoundUpToPowerOfTwo(static_cast<uint32>(MaxTimeStep * 1024.0))
		>> 1) / 1024.0);
}

FReply SSSussCurveVisualization::OnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	/*const bool bRightMouseButton = InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton;

	if (bRightMouseButton)
	{
		MouseDownLocation = InMyGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}*/
	
	return FReply::Unhandled();
}

FReply SSSussCurveVisualization::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const FVector2D ZoomDelta = -0.1f * FVector2D(MouseEvent.GetWheelDelta());

	if (!MouseEvent.GetModifierKeys().IsShiftDown() || MouseEvent.GetModifierKeys().IsControlDown())
	{
		const float OutputViewSize = ViewMaxOutput.Get() - ViewMinOutput.Get();
		const float OutputChange = OutputViewSize * ZoomDelta.Y;

		const float NewMinOutput = (ViewMinOutput.Get() - (OutputChange * 0.5f));
		const float NewMaxOutput = (ViewMaxOutput.Get() + (OutputChange * 0.5f));

		ViewMinOutput.Set(NewMinOutput);
		ViewMaxOutput.Set(NewMaxOutput);
	}

	if (MouseEvent.GetModifierKeys().IsShiftDown() || MouseEvent.GetModifierKeys().IsControlDown())
	{
		const float InputViewSize = ViewMaxInput.Get() - ViewMinInput.Get();
		const float InputChange = InputViewSize * ZoomDelta.X;

		const float NewMinInput = ViewMinInput.Get() - (InputChange * 0.5f);
		const float NewMaxInput = ViewMaxInput.Get() + (InputChange * 0.5f);

		ViewMinInput.Set(NewMinInput);
		ViewMaxInput.Set(NewMaxInput);
	}

	return FReply::Handled();
}

FReply SSSussCurveVisualization::ZoomToFitHorizontalClicked()
{
	ViewMinInput.Set(DefaultMinInput);
	ViewMaxInput.Set(DefaultMaxInput);

	return FReply::Handled();
}

FReply SSSussCurveVisualization::ZoomToFitVerticalClicked()
{
	ViewMinOutput.Set(DefaultMinOutput);
	ViewMaxOutput.Set(DefaultMaxOutput);

	return FReply::Handled();
}

void SSSussCurveVisualization::PaintGridLines(
	const FGeometry& AllottedGeometry,
	FTrackScaleInfo& ScaleInfo,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FSlateRect& MyCullingRect,
	ESlateDrawEffect DrawEffects) const
{
	constexpr float MaxGridPixelSpacing = 150.0f;
	const float GridPixelSpacing = FMath::Min(ScaleInfo.WidgetSize.GetMin() / 1.5f, MaxGridPixelSpacing);
	constexpr FLinearColor GridTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.75f);
	constexpr FLinearColor GridColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.3f);
	constexpr FLinearColor GridColor_Recessed = FLinearColor(0.0f, 0.0f, 0.0f, 0.1f);

	{
		float TimeStep = GetTimeStep(ScaleInfo);
		float ScreenStepTime = ScaleInfo.InputToLocalX(TimeStep) - ScaleInfo.InputToLocalX(0.0f);

		if (ScreenStepTime >= 1.0f)
		{
			float StartTime = ScaleInfo.LocalXToInput(0.0f);
			TArray<FVector2D> LinePoints;
			float ScaleX = (TimeStep) / (AllottedGeometry.GetLocalSize().X);

			//draw vertical grid lines
			float StartOffset = -FMath::Fractional(StartTime / TimeStep) * ScreenStepTime;
			float Time = ScaleInfo.LocalXToInput(StartOffset);
			for (float X = StartOffset; X < AllottedGeometry.GetLocalSize().X; X += ScreenStepTime, Time += TimeStep)
			{
				if (X < 0.0f || X > AllottedGeometry.GetLocalSize().X)
				{
					continue;
				}

				if (SMALL_NUMBER < FMath::Abs(X))
				{
					LinePoints.Add(FVector2D(X, 0.f));
					LinePoints.Add(FVector2D(X, AllottedGeometry.GetLocalSize().Y));
					FSlateDrawElement::MakeLines(
						OutDrawElements,
						LayerId,
						AllottedGeometry.ToPaintGeometry(),
						LinePoints,
						DrawEffects,
						GridColor,
						false);

					//Show grid time
					FString TimeStr = FString::Printf(TEXT("%.2f"), Time);
					FSlateDrawElement::MakeText(OutDrawElements, LayerId,
					                            AllottedGeometry.MakeChild(
						                            FVector2D(1.0f, ScaleX),
						                            FSlateLayoutTransform(FVector2D(X, 0.0))).ToPaintGeometry(),
					                            TimeStr,
					                            FAppStyle::GetFontStyle("CurveEd.InfoFont"), DrawEffects,
					                            GridTextColor);

					LinePoints.Empty();
				}
			}

			// Draw horizontal OutputStrings
			{
				const float OutputRange = ScaleInfo.ViewMaxOutput - ScaleInfo.ViewMinOutput;
				const float OutputLabelStep = OutputRange / 10.f;

				for (float Output = ScaleInfo.ViewMinOutput; Output <= ScaleInfo.ViewMaxOutput + KINDA_SMALL_NUMBER;
				     Output += OutputLabelStep)
				{
					float Y = ScaleInfo.OutputToLocalY(Output);
					if (Y >= 0.0f && Y <= AllottedGeometry.GetLocalSize().Y)
					{
						LinePoints.Add(FVector2D(0.f, Y));
						LinePoints.Add(FVector2D(AllottedGeometry.GetLocalSize().X, Y));
						FSlateDrawElement::MakeLines(
							OutDrawElements,
							LayerId,
							AllottedGeometry.ToPaintGeometry(),
							LinePoints,
							DrawEffects,
							GridColor_Recessed,
							false);
						LinePoints.Empty();

						FString OutputStr = FString::Printf(TEXT("%.1f"), Output);
						FSlateDrawElement::MakeText(
							OutDrawElements,
							LayerId,
							AllottedGeometry.MakeChild(
								FVector2D(ScaleX, 1.0f),
								FSlateLayoutTransform(FVector2D(2.0f, Y))
							).ToPaintGeometry(),
							OutputStr,
							FAppStyle::GetFontStyle("CurveEd.InfoFont"),
							DrawEffects,
							GridTextColor
						);
					}
				}
			}
		}
	}
}

void SSSussCurveVisualization::PaintCurve(
	const FGeometry& AllottedGeometry,
	FTrackScaleInfo& ScaleInfo,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FSlateRect& MyCullingRect,
	ESlateDrawEffect DrawEffects,
	const FWidgetStyle& InWidgetStyle,
	bool bAnyCurveViewModelsSelected) const
{
	const FLinearColor CurveColor = FColor::Red;

	TArray<FVector2D> CurvePoints;

	constexpr float CurveSampleStep = 0.01f;
	TArray<TPair<float, float>> CurveSamples;
	CurveSamples.Reserve(static_cast<int32>(1.f / CurveSampleStep));

	// Sample the curve
	const FSussConsideration Consideration = OnGetConsideration.Execute();
	for (float X = 0.f; X <= 1.f + CurveSampleStep; X += CurveSampleStep)
	{
		float Y = Consideration.EvaluateCurve(X);

		CurveSamples.Add(TPair<float, float>(X, Y));
	}

	// Add arrive and exit lines, skip if out of bounds
	// {
	// 	float ArriveX = ScaleInfo.InputToLocalX(CurveSamples[0].Key);
	// 	float ArriveY = ScaleInfo.OutputToLocalY(CurveSamples[0].Value);
	// 	float LeaveX = ScaleInfo.InputToLocalX(CurveSamples.Last().Key);
	// 	float LeaveY = ScaleInfo.OutputToLocalY(CurveSamples.Last().Value);
	// 	float WidgetWidth = AllottedGeometry.GetLocalSize().X;
	// 	float WidgetHeight = AllottedGeometry.GetLocalSize().Y;
	//
	// 	// Arrival
	// 	if (ArriveX >= 0.0f && ArriveX <= WidgetWidth &&
	// 		ArriveY >= 0.0f && ArriveY <= WidgetHeight)
	// 	{
	// 		CurvePoints.Add(FVector2D(0.0f, ArriveY));
	// 		CurvePoints.Add(FVector2D(ArriveX, ArriveY));
	// 		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CurvePoints,
	// 									 DrawEffects, CurveColor);
	// 		CurvePoints.Empty();
	// 	}
	// 	// LEave
	// 	if (LeaveX >= 0.0f && LeaveX <= WidgetWidth &&
	// 		LeaveY >= 0.0f && LeaveY <= WidgetHeight)
	// 	{
	// 		CurvePoints.Add(FVector2D(WidgetWidth, LeaveY));
	// 		CurvePoints.Add(FVector2D(LeaveX, LeaveY));
	// 		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CurvePoints,
	// 									 DrawEffects, CurveColor);
	// 		CurvePoints.Empty();
	// 	}
	// }

	// Make Curve points
	for (const TPair<float, float>& Sample : CurveSamples)
	{
		float X = ScaleInfo.InputToLocalX(Sample.Key);
		float Y = ScaleInfo.OutputToLocalY(Sample.Value);

		if (X >= 0.0f && X <= AllottedGeometry.GetLocalSize().X &&
			Y >= 0.0f && Y <= AllottedGeometry.GetLocalSize().Y)
		{
			CurvePoints.Add(FVector2D(X, Y));
		}

		//CurvePoints.Add(FVector2D(X, Y));
	}

	// Draw the curve lines
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		CurvePoints,
		DrawEffects,
		CurveColor,
		true);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
