#include "SussConsideration.h"
#include "Customization/SussConsiderationCustomization.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSUSSEditorModule"

class FSUSSEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
IMPLEMENT_MODULE(FSUSSEditorModule, SUSSEditor)


void FSUSSEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditor.RegisterCustomPropertyTypeLayout(FSussConsideration::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSussConsiderationCustomization::MakeInstance));
	PropertyEditor.NotifyCustomizationModuleChanged();
}

void FSUSSEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE