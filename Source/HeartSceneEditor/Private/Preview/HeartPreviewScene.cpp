// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Preview/HeartPreviewScene.h"
#include "Preview/PreviewSceneConfig.h"

#include "HeartSceneActor.h"
#include "HeartSceneGenerator.h"

#include "Model/HeartGraph.h"
#include "Graph/HeartGraphAssetEditor.h"

#include "GameFramework/WorldSettings.h"

namespace Heart::AssetEditor
{
	FHeartPreviewScene::FHeartPreviewScene(ConstructionValues CVS, const TSharedRef<FHeartGraphEditor>& EditorToolkit)
		: FAdvancedPreviewScene(CVS)
		, EditorPtr(EditorToolkit)
	{
		// Disable killing actors outside of the world
		AWorldSettings* WorldSettings = GetWorld()->GetWorldSettings(true);
		WorldSettings->bEnableWorldBoundsChecks = false;

		//Hide default floor
		SetFloorVisibility(false, false);
	}

	FHeartPreviewScene::~FHeartPreviewScene()
	{
	}

	void FHeartPreviewScene::Tick(const float InDeltaTime)
	{
		FAdvancedPreviewScene::Tick(InDeltaTime);

		// Tick the scene world
		GetWorld()->Tick(LEVELTICK_All, InDeltaTime);
	}

	void FHeartPreviewScene::OnRefresh()
	{
		if (!IsValid(SceneConfig))
		{
			// Add component that exposed settings for configuring the scene
			SceneConfig = NewObject<UPreviewSceneConfig>(GetTransientPackage());
			FAdvancedPreviewScene::AddComponent(SceneConfig, FTransform::Identity);

			SceneConfig->OnConfigEdit.BindRaw(this, &FHeartPreviewScene::OnConfigEdit);
		}
	}

	void FHeartPreviewScene::OnConfigEdit(const FPropertyChangedEvent& PropertyChangedEvent)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UPreviewSceneConfig, SceneClass))
		{
			ReconstructSceneActor();
		}
	}

	void FHeartPreviewScene::ReconstructSceneActor()
	{
		if (SceneActor)
		{
			SceneActor->Destroy();
			SceneActor = nullptr;
		}

		if (IsValid(SceneConfig->SceneClass))
		{
			FActorSpawnParameters Params;
			SceneActor = GetWorld()->SpawnActor<AHeartSceneActor>(SceneConfig->SceneClass);

			if (auto&& Generator = SceneActor->GetGenerator())
			{
				if (UHeartGraph* Graph = DuplicateObject<UHeartGraph>(EditorPtr.Pin()->GetHeartGraph(), SceneActor))
				{
					Generator->SetDisplayedGraph(Graph);
					{
						FEditorScriptExecutionGuard EditorScriptExecutionGuard;
						Generator->Regenerate();
					}
				}
			}
		}
	}
}
