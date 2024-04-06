﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "Input/HeartWidgetInputLinker.h"
#include "HeartCanvasLog.h"
#include "Components/Widget.h"
#include "Input/HeartDragDropOperation.h"

#include "Input/HeartInputActivation.h"
#include "Input/HeartInputLinkerInterface.h"
#include "Input/HeartInputTypes.h"
#include "General/HeartContextObject.h"
#include "Input/HeartEvent.h"
#include "Input/HeartSlateReplyWrapper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HeartWidgetInputLinker)

using namespace Heart::Input;

FReply UHeartWidgetInputLinker::HandleOnMouseWheel(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseWheel)

	// @todo what the heck went on here? does PointerEvent have a different/null EffectingButton?
	const FPointerEvent HackedPointerEvent = FPointerEvent(
		PointerEvent.GetUserIndex(),
		PointerEvent.GetPointerIndex(),
		PointerEvent.GetScreenSpacePosition(),
		PointerEvent.GetLastScreenSpacePosition(),
		PointerEvent.GetPressedButtons(),
		EKeys::MouseWheelAxis,
		PointerEvent.GetWheelDelta(),
		PointerEvent.GetModifierKeys());

	// Mouse wheel events must always use the 'Press' type
	const FHeartEvent Reply = TryCallbacks(FInputTrip(HackedPointerEvent, Press), Widget, HackedPointerEvent);

	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonDown(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonDown)

	const FInputTrip Trip(PointerEvent, Press);

	const FHeartEvent Reply = TryCallbacks(Trip, Widget, PointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	// If no regular handles triggered, try DDO triggers.

	TArray<const TSharedPtr<const FConditionalCallback_DDO>*> DropDropTriggerArray;
	DragDropTriggers.MultiFindPointer(Trip, DropDropTriggerArray);
	Algo::Sort(DropDropTriggerArray,
		[](const TSharedPtr<const FConditionalCallback_DDO>* A, const TSharedPtr<const FConditionalCallback_DDO>* B)
		{
			return *A->Get() < *B->Get();
		});

	for (auto&& CallbackPtr : DropDropTriggerArray)
	{
		const FConditionalCallback_DDO& Ref = *CallbackPtr->Get();

		bool PassedCondition = true;

		if (Ref.Condition.IsBound())
		{
			PassedCondition = Ref.Condition.Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		const auto HandlerCallback = static_cast<TLinkerType<UWidget>::FCreateDDODelegate*>(Ref.Handler.Get());
		if (!HandlerCallback->IsBound())
		{
			continue;
		}

		if (const TSharedPtr<SWidget> SlateWidgetDetectingDrag = Widget->GetCachedWidget();
			SlateWidgetDetectingDrag.IsValid())
		{
			FReply DDO_Reply = Ref.Priority == Event ? FReply::Handled() : FReply::Unhandled();

			return DDO_Reply.DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), PointerEvent.GetEffectingButton());
		}
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnMouseButtonUp(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnMouseButtonUp)

	const FHeartEvent Reply = TryCallbacks(FInputTrip(PointerEvent, Release), Widget, PointerEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyDown(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyDown)

	const FHeartEvent Reply = TryCallbacks(FInputTrip(KeyEvent, Press), Widget, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

FReply UHeartWidgetInputLinker::HandleOnKeyUp(UWidget* Widget, const FGeometry& InGeometry, const FKeyEvent& KeyEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnKeyUp)

	const FHeartEvent Reply = TryCallbacks(FInputTrip(KeyEvent, Release), Widget, KeyEvent);
	if (auto EventReply = Reply.As<FEventReply>();
		EventReply.IsSet())
	{
		return EventReply.GetValue().NativeReply;
	}

	return FReply::Unhandled();
}

UHeartDragDropOperation* UHeartWidgetInputLinker::HandleOnDragDetected(UWidget* Widget, const FGeometry& InGeometry, const FPointerEvent& PointerEvent)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleOnDragDetected)

	TArray<const TSharedPtr<const FConditionalCallback_DDO>*> DropDropTriggerArray;
	DragDropTriggers.MultiFindPointer(FInputTrip(PointerEvent, Press), DropDropTriggerArray);
	Algo::Sort(DropDropTriggerArray,
		[](const TSharedPtr<const FConditionalCallback_DDO>* A, const TSharedPtr<const FConditionalCallback_DDO>* B)
		{
			return *A->Get() < *B->Get();
		});

	for (auto&& CallbackPtr : DropDropTriggerArray)
	{
		const FConditionalCallback_DDO& Ref = *CallbackPtr->Get();

		bool PassedCondition = true;

		if (Ref.Condition.IsBound())
		{
			PassedCondition = Ref.Condition.Execute(Widget);
		}

		if (!PassedCondition)
		{
			continue;
		}

		const auto HandlerCallback = static_cast<TLinkerType<UWidget>::FCreateDDODelegate*>(Ref.Handler.Get());
		if (!HandlerCallback->IsBound())
		{
			continue;
		}

		UHeartDragDropOperation* DragDropOperation = HandlerCallback->Execute(Widget);

		if (!IsValid(DragDropOperation)) continue;

		DragDropOperation->SummonedBy = Widget;

		if (Widget->Implements<UHeartContextObject>())
		{
			DragDropOperation->Payload = IHeartContextObject::Execute_GetContextObject(Widget);
		}

		if (DragDropOperation->SetupDragDropOperation())
		{
			return DragDropOperation;
		}
		else
		{
			UE_LOG(LogHeartCanvas, Warning, TEXT("Created DDO (%s) unnecessarily, figure out why"), *DragDropOperation->GetClass()->GetName())
		}
	}

	return nullptr;
}

bool UHeartWidgetInputLinker::HandleNativeOnDragOver(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent, UDragDropOperation* InOperation)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDragOver)

	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->OnHoverWidget(Widget);
	}

	return false;
}

bool UHeartWidgetInputLinker::HandleNativeOnDrop(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
												 UDragDropOperation* InOperation)
{
	//SCOPE_CYCLE_COUNTER(STAT_HandleNativeOnDrop)

	if (auto&& HeartDDO = Cast<UHeartDragDropOperation>(InOperation))
	{
		return HeartDDO->CanDropOnWidget(Widget);
	}

	return false;
}

void UHeartWidgetInputLinker::HandleNativeOnDragEnter(UWidget* Widget, const FGeometry& InGeometry, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleNativeOnDragLeave(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::HandleNativeOnDragCancelled(UWidget* Widget, const FDragDropEvent& DragDropEvent,
	UDragDropOperation* InOperation)
{
	// Nothing here yet
}

void UHeartWidgetInputLinker::BindToOnDragDetected(const FInputTrip& Trip, const TSharedPtr<const FConditionalCallback_DDO>& DragDropTrigger)
{
	if (ensure(Trip.IsValid()))
	{
		DragDropTriggers.Add(Trip, DragDropTrigger);
	}
}

void UHeartWidgetInputLinker::UnbindToOnDragDetected(const FInputTrip& Trip)
{
	DragDropTriggers.Remove(Trip);
}

namespace Heart::Input
{
	UHeartWidgetInputLinker* TLinkerType<UWidget>::FindLinker(const UWidget* Widget)
	{
		return TryFindLinker<UHeartWidgetInputLinker>(Widget);
	}
}