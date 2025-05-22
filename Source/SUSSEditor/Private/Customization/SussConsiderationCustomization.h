// Author: Tom Werner (MajorT), 2025

#pragma once

#include "IDetailCustomization.h"

struct FSussConsideration;
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
	FSussConsideration GetConsideration() const;

private:
	TSharedPtr<IPropertyHandle> MyStructHandle;
	TSharedPtr<SSSussCurveVisualization> CurveVisualization;
};
