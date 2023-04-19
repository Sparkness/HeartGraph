﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGraphPinType.h"
#include "HeartGraphPinDesc.h"
#include "Model/HeartGuids.h"
#include "Model/HeartGraphPinReference.h"
#include "Model/HeartPinDirection.h"
#include "HeartGraphPin.generated.h"

class UHeartGraphNode;
class UHeartGraphPin;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartPinConnectionsChanged, UHeartGraphPin*, Pin);

/**
 *
 */
UCLASS(BlueprintType)
class HEART_API UHeartGraphPin : public UObject // Based on UEdGraphPin
{
	GENERATED_BODY()

	friend UHeartGraphNode;
	friend class UHeartEdGraphNode;

public:
	bool ConnectTo(UHeartGraphPin* Other);

	void DisconnectFrom(const FHeartGraphPinReference Other, bool NotifyNode);

	void DisconnectFromAll(bool NotifyNodes);


	/*----------------------------
			REFLECTION
	----------------------------*/

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartGraphPinDesc GetPinDesc() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FName GetPinName() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	const FText& GetFriendlyName() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	const FText& GetToolTip() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartGraphPinType GetType() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	EHeartPinDirection GetDirection() const;

	/*----------------------------
				GETTERS
	----------------------------*/
protected:
#if WITH_EDITOR
	// @todo this data needs to be exposed better. maybe not even made here, but using out data. what about UHeartEdGraphNode make the FEdGraphPinType
	FEdGraphPinType GetPinType() const;
#endif

public:
	template <typename THeartGraphNode>
	THeartGraphNode* GetOwningNode() const
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The graph node class must derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(GetOuter());
	}

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	UHeartGraphNode* GetNode() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin", meta = (DeterminesOutputType = "Class"))
	UHeartGraphNode* GetNodeTyped(TSubclassOf<UHeartGraphNode> Class) const { return GetNode(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartPinGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	FHeartGraphPinReference GetReference() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	const TArray<FHeartGraphPinReference>& GetLinks() const { return Links; }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	bool IsConnected() const { return !Links.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	int32 GetNumLinks() const { return Links.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Heart|GraphPin")
	TArray<UHeartGraphPin*> GetAllConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphPin")
	UHeartGraphPin* ResolveConnectionByReference(const FHeartGraphPinReference Reference) const;

	/*
	 * Gets a connection pin for the given index. Treat this like an array, only access indices you know to be valid.
	 * Use GetNumLinks or IsConnected to determine this first.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|GraphPin")
	UHeartGraphPin* ResolveConnection(const int32 Index) const;

public:
	UPROPERTY(BlueprintAssignable, Transient, Category = "Events")
	FHeartPinConnectionsChanged OnPinConnectionsChanged;

private:
	UPROPERTY(BlueprintReadOnly)
	FHeartPinGuid Guid;

	UPROPERTY(BlueprintReadOnly)
	FHeartGraphPinDesc PinDesc;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FHeartGraphPinReference> Links;
};
