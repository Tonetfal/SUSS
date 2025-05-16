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
	CurveTypeHandle = MyStructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSussConsideration, CurveType));
	CurveParamsHandle = MyStructHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FSussConsideration, CurveParams));

	
	
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
			.OnGetCurveParams(this, &FSussConsiderationCustomization::GetCurveParams)
			.OnGetCurveType(this, &FSussConsiderationCustomization::GetCurveType)
			.ViewMinInput(0.f)
			.ViewMaxInput(1.f)
			.ViewMinOutput(0.f)
			.ViewMaxOutput(1.f)
		]
	];
}

FVector4f FSussConsiderationCustomization::GetCurveParams() const
{
	const FVector4f* CurveParams = nullptr;
	{
		checkf(CurveParamsHandle, TEXT("No Curve Params Handle :(( "))
		void* Data = nullptr;
		if (CurveParamsHandle->GetValueData(Data) != FPropertyAccess::Fail)
		{
			CurveParams = static_cast<FVector4f*>(Data);
		}
	}

	return *CurveParams;
}

ESussCurveType FSussConsiderationCustomization::GetCurveType() const
{
	const ESussCurveType* CurveType = nullptr;
	{
		checkf(CurveTypeHandle, TEXT("No Curve  Type Handle :(( "))
		void* Data = nullptr;
		if (CurveTypeHandle->GetValueData(Data) != FPropertyAccess::Fail)
		{
			CurveType = static_cast<ESussCurveType*>(Data);
		}
	}

	return *CurveType;
}


#undef LOCTEXT_NAMESPACE
