// Author: Tom Werner (MajorT), 2025

#pragma once

#include "CoreMinimal.h"
#include "SussCommon.h"
#include "Widgets/SCompoundWidget.h"

struct FTrackScaleInfo;

/** Lol this name is so bad */
class SSSussCurveVisualization : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal(FVector4f, FOnGetCurveParams);
	DECLARE_DELEGATE_RetVal(ESussCurveType, FOnGetCurveType);
	
	SLATE_BEGIN_ARGS(SSSussCurveVisualization)
		: _DesiredSize(FVector2D::ZeroVector)
		, _ViewMinInput(0.0f)
		, _ViewMaxInput(1.0f)
		, _ViewMinOutput(0.0f)
		, _ViewMaxOutput(1.0f)
		{
		}
		SLATE_ATTRIBUTE(FVector2D, DesiredSize)
		SLATE_ATTRIBUTE(float, ViewMinInput)
		SLATE_ATTRIBUTE(float, ViewMaxInput)
		SLATE_ATTRIBUTE(float, ViewMinOutput)
		SLATE_ATTRIBUTE(float, ViewMaxOutput)
		SLATE_EVENT(FOnGetCurveParams, OnGetCurveParams)
		SLATE_EVENT(FOnGetCurveType, OnGetCurveType)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

protected:
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual float GetTimeStep(FTrackScaleInfo &ScaleInfo) const;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FReply ZoomToFitHorizontalClicked();
	FReply ZoomToFitVerticalClicked();

protected:
	void PaintGridLines(const FGeometry& AllottedGeometry, FTrackScaleInfo& ScaleInfo,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FSlateRect& MyCullingRect,
		ESlateDrawEffect DrawEffects) const;
	
	void PaintCurve(const FGeometry& AllottedGeometry, FTrackScaleInfo& ScaleInfo,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FSlateRect& MyCullingRect,
		ESlateDrawEffect DrawEffects, const FWidgetStyle& InWidgetStyle, bool bAnyCurveViewModelsSelected) const;

private:
	TAttribute<FVector2D> DesiredSize;
	FOnGetCurveParams OnGetCurveParams;
	FOnGetCurveType OnGetCurveType;

	float DefaultMinInput = 0.f;
	float DefaultMaxInput = 0.f;
	float DefaultMinOutput = 0.f;
	float DefaultMaxOutput = 0.f;
	TAttribute<float> ViewMinInput;
	TAttribute<float> ViewMaxInput;
	TAttribute<float> ViewMinOutput;
	TAttribute<float> ViewMaxOutput;

	FVector2D MouseDownLocation = FVector2D::ZeroVector;
};
