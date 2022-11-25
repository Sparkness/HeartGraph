// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Nodes/AssetTypeActions_HeartGraphNodeBlueprint.h"
#include "Nodes/HeartGraphNodeBlueprintFactory.h"
#include "HeartEditorModule.h"
#include "ModelView/HeartGraphNode.h"

#include "ModelView/HeartGraphNodeBlueprint.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_HeartGraphNodeBlueprint"

FText FAssetTypeActions_HeartGraphNodeBlueprint::GetName() const
{
	return LOCTEXT("AssetTypeActions_HeartGraphNodeBlueprint", "Heart Graph Node Blueprint");
}

uint32 FAssetTypeActions_HeartGraphNodeBlueprint::GetCategories()
{
	return FHeartEditorModule::HeartAssetCategory;
}

UClass* FAssetTypeActions_HeartGraphNodeBlueprint::GetSupportedClass() const
{
	return UHeartGraphNodeBlueprint::StaticClass();
}

UFactory* FAssetTypeActions_HeartGraphNodeBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	auto&& HeartGraphNodeBlueprintFactory = NewObject<UHeartGraphNodeBlueprintFactory>();
	HeartGraphNodeBlueprintFactory->ParentClass = TSubclassOf<UHeartGraphNode>(*InBlueprint->GeneratedClass);
	return HeartGraphNodeBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE
