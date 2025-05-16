using UnrealBuildTool;

public class SUSSEditor : ModuleRules
{
    public SUSSEditor(ReadOnlyTargetRules target) : base(target)
    {
        PublicDependencyModuleNames.AddRange(new []
        { 
            "Core",
        });

        PrivateDependencyModuleNames.AddRange(new []
        { 
            "CoreUObject", 
            "Engine", 
            "Slate", 
            "SlateCore",
            "SUSS",
            "UnrealEd",
            "PropertyEditor",
        });
    }
}