#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(TargetSpawnerTest, "BeatShot.BeatShot.Private.TargetSpawnerTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void TargetSpawnerTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	FAutomationEditorCommonUtils::LoadMap("Range_WP");
}

bool TargetSpawnerTest::RunTest(const FString& Parameters)
{
	//FFunctionLatentCommand
	return true;
}
