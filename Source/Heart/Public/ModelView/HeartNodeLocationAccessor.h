﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HeartNodeLocationAccessor.generated.h"

struct FHeartNodeGuid;
class UHeartGraphNode;
class UHeartGraphNode3D;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class HEART_API UHeartNodeLocationAccessor : public UInterface
{
	GENERATED_BODY()
};

/**
 * This must be added to classes that display node positions visually, in order for other systems (like layout
 * automation) to interop with them.
 */
class HEART_API IHeartNodeLocationAccessor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor")
	virtual const UHeartGraph* GetHeartGraph() const PURE_VIRTUAL(IHeartNodeLocationAccessor::GetHeartGraph, return nullptr; )

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location (Guid)"))
	virtual FVector2D GetNodeLocation(FHeartNodeGuid Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location (Guid)"))
	virtual void SetNodeLocation(FHeartNodeGuid Node, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D (Guid)"))
	virtual FVector GetNodeLocation3D(FHeartNodeGuid Node) const;

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D (Guid)"))
	virtual void SetNodeLocation3D(FHeartNodeGuid Node, const FVector& Location);
};

UCLASS()
class UHeartNodeLocationAccessorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location"))
	static FVector2D GetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location"))
	static void SetNodeLocation_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode* Node, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Get Node Location 3D"))
	static FVector GetNodeLocation3D_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode3D* Node);

	UFUNCTION(BlueprintCallable, Category = "HeartNodePositionAccessor", meta = (DisplayName = "Set Node Location 3D"))
	static void SetNodeLocation3D_Pointer(const TScriptInterface<IHeartNodeLocationAccessor>& Accessor, UHeartGraphNode3D* Node, const FVector& Location);
};

/*
 * A pass-through class for redirecting an accessor through proxy function to alter the location.
 */
UCLASS()
class UHeartNodeLocationProxy : public UObject, public IHeartNodeLocationAccessor
{
	GENERATED_BODY()

public:
	/* IHeartNodeLocationAccessor */
	virtual const UHeartGraph* GetHeartGraph() const override final;
	virtual FVector2D GetNodeLocation(FHeartNodeGuid Node) const override final;
	virtual void SetNodeLocation(FHeartNodeGuid Node, const FVector2D& Location) override final;
	virtual FVector GetNodeLocation3D(FHeartNodeGuid Node) const override final;
	virtual void SetNodeLocation3D(FHeartNodeGuid Node, const FVector& Location) override final;
	/* IHeartNodeLocationAccessor */

protected:
	virtual FVector2D LocationToProxy(const FVector2D& Location) const;
	virtual FVector2D ProxyToLocation(const FVector2D& Proxy) const;

	virtual FVector LocationToProxy3D(const FVector& Location) const;
	virtual FVector ProxyToLocation3D(const FVector& Proxy) const;

public:
	static UHeartNodeLocationProxy* Create(UObject* ObjectToProxy, TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass);

protected:
	UPROPERTY()
	TScriptInterface<IHeartNodeLocationAccessor> ProxiedObject;
};


UCLASS()
class UHeartNodeLocationProxyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Heart|NodeLocationProxyLibrary")
	static UHeartNodeLocationProxy* CreateNodeLocationProxy(UObject* ObjectToProxy, TSubclassOf<UHeartNodeLocationProxy> LocationProxyClass);
};
