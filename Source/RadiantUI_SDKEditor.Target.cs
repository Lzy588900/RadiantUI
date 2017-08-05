

using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantUI_SDKEditorTarget : TargetRules
{
	public RadiantUI_SDKEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        ExtraModuleNames.Add("RadiantUI_SDK");
	}
}
