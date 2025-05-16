// Author: Tom Werner (MajorT), 2025

#pragma once

#include "IDetailCustomization.h"

enum class ESussCurveType : uint8;
class SSSussCurveVisualization;
class SCurveEditor;

class FSussConsiderationCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	//~ End IDetailCustomization Interface

private:
	FVector4f GetCurveParams() const;
	ESussCurveType GetCurveType() const;

private:
	TSharedPtr<IPropertyHandle> MyStructHandle;
	TSharedPtr<IPropertyHandle> CurveParamsHandle;
	TSharedPtr<IPropertyHandle> CurveTypeHandle;
	TSharedPtr<SSSussCurveVisualization> CurveVisualization;
};
