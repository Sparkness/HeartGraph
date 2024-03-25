﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Actions/HeartCanvasActionDragDropOperation.h"
#include "Actions/HeartGraphCanvasAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "UI/HeartUMGContextObject.h"
#include "UMG/HeartGraphWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartCanvasActionDragDropOperation)

bool UHeartCanvasActionDragDropOperation::SetupDragDropOperation()
{
	return IsValid(Action);
}

void UHeartCanvasActionDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	if (IsValid(Action))
	{
		if (Action->CanExecute(GetHoveredWidget()))
		{
			Heart::Action::FArguments Args;
			Args.Target = GetHoveredWidget();
			Args.Activation = PointerEvent;
			Args.Payload = Payload;

			Action->Execute(Args);
		}
	}
}

bool UHeartCanvasInputHandler_DDO_Action::PassCondition(const UWidget* TestWidget) const
{
	bool Failed = !Super::PassCondition(TestWidget);

	if (IsValid(ActionClass))
	{
		Failed |= !ActionClass.GetDefaultObject()->CanExecute(TestWidget);
	}

	return !Failed;
}

UHeartDragDropOperation* UHeartCanvasInputHandler_DDO_Action::BeginDDO(UWidget* Widget) const
{
	auto&& NewDDO = NewObject<UHeartCanvasActionDragDropOperation>(GetTransientPackage());

	if (IsValid(VisualClass))
	{
		auto&& NewVisual = CreateWidget(Widget, VisualClass);

		// If both the widget and the visual want a context object pass it between them
		if (Widget->Implements<UHeartUMGContextObject>() &&
			NewVisual->Implements<UHeartUMGContextObject>())
		{
			auto&& Context = IHeartUMGContextObject::Execute_GetContextObject(Widget);
			IHeartUMGContextObject::Execute_SetContextObject(NewVisual, Context);
		}

		NewDDO->DefaultDragVisual = NewVisual;
		NewDDO->Pivot = Pivot;
		NewDDO->Offset = Offset;
	}

	NewDDO->Action = NewObject<UHeartGraphCanvasAction>(NewDDO, ActionClass);

	return NewDDO;
}