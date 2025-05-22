// Author: Tom Werner (MajorT), 2025


#include "SussConsiderationCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "SCurveEditor.h"
#include "SussConsideration.h"
#include "Widgets/SSSussCurveVisualization.h"

#define LOCTEXT_NAMESPACE "FSussConsiderationCustomization"

TSharedRef<IPropertyTypeCustomization> FSussConsiderationCustomization::MakeInstance()
{
	return MakeShared<FSussConsiderationCustomization>();
}

void FSussConsiderationCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle,
	FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	MyStructHandle = PropertyHandle;

	HeaderRow
	.NameContent()
	[
		MyStructHandle->CreatePropertyNameWidget()
	];
}

void FSussConsiderationCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// Add existing children
	uint32 NumChildren;
	MyStructHandle->GetNumChildren(NumChildren);
	for (uint32 i = 0; i < NumChildren; ++i)
	{
		ChildBuilder.AddProperty(MyStructHandle->GetChildHandle(i).ToSharedRef());
	}

	// Add our own visualizer 
	FDetailWidgetRow& MyRow = ChildBuilder.AddCustomRow(LOCTEXT("GroupName", "Curve Visualization"));
	MyRow
	.NameContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CurveVis", "Preview Curve"))
			.Font(CustomizationUtils.GetRegularFont())
		]
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	.MinDesiredWidth(200.f)
	[
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(CurveVisualization, SSSussCurveVisualization)
			.DesiredSize(FVector2D(300, 200))
			.OnGetConsideration(this, &FSussConsiderationCustomization::GetConsideration)
			.ViewMinInput(0.f)
			.ViewMaxInput(1.0f)
			.ViewMinOutput(0.f)
			.ViewMaxOutput(1.0f)
		]
	];
}

FSussConsideration FSussConsiderationCustomization::GetConsideration() const
{
	if (ensureMsgf(MyStructHandle, TEXT("Struct handle is missing")))
	{
		void* Data = nullptr;
		if (MyStructHandle->GetValueData(Data) != FPropertyAccess::Fail)
		{
			const FSussConsideration* Consideration = static_cast<FSussConsideration*>(Data);
			return *Consideration;
		}
	}

	return {};
}

#undef LOCTEXT_NAMESPACE
