﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#include "BloodContainer.h"
#include "BloodValue.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BloodContainer)

// #@todo this is all... very questionable

void FBloodContainer::AddBloodValue(const FName Name, const FBloodValue& Value)
{
	if (Value.IsContainer2())
	{
		if (Value.PropertyBag.GetNumPropertiesInBag() != 2)
		{
			// Invalid Descs number, abort
			return;
		}

		FPropertyBagPropertyDesc DescV0 = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		FPropertyBagPropertyDesc DescV1 = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[1];

		const uint8* Memory = Value.GetMemory();

		FNameBuilder NameBuilder[2];
		Name.ToString(NameBuilder[0]);
		Name.ToString(NameBuilder[1]);
		NameBuilder[0].Append(TEXT("__V0"));
		NameBuilder[1].Append(TEXT("__V1"));

		DescV0.Name = FName(NameBuilder[0]);
		DescV1.Name = FName(NameBuilder[1]);

		PropertyBag.AddProperties({DescV0, DescV1});

		PropertyBag.SetValue(DescV0.Name, DescV0.CachedProperty, Memory);
		PropertyBag.SetValue(DescV1.Name, DescV1.CachedProperty, Memory);
	}
	else
	{
		if (Value.PropertyBag.GetNumPropertiesInBag() != 1)
		{
			// Invalid Descs number, abort
			return;
		}

		FPropertyBagPropertyDesc DescV0 = Value.PropertyBag.GetPropertyBagStruct()->GetPropertyDescs()[0];
		DescV0.Name = Name;

		const uint8* Memory = Value.GetMemory();

		PropertyBag.AddProperties({DescV0});
		PropertyBag.SetValue(Name, DescV0.CachedProperty, Memory);
	}
}

void FBloodContainer::Remove(const FName Name)
{
	TArray<FName> Names { Name };

	FNameBuilder NameBuilder[2];
	Name.ToString(NameBuilder[0]);
	Name.ToString(NameBuilder[1]);
	NameBuilder[0].Append(TEXT("__V0"));
	NameBuilder[1].Append(TEXT("__V1"));
	Names.Emplace(NameBuilder[0]);
	Names.Emplace(NameBuilder[1]);

	PropertyBag.RemovePropertiesByName(Names);
}

void FBloodContainer::Clear()
{
	PropertyBag.Reset();
}

TOptional<FBloodValue> FBloodContainer::GetBloodValue(const FName Name) const
{
	if (auto&& ExactDesc = PropertyBag.FindPropertyDescByName(Name))
	{
		FInstancedPropertyBag Temp;
		FPropertyBagPropertyDesc CopyDesc = *ExactDesc;
		CopyDesc.Name = Blood::Private::V0;
		Temp.AddProperties({CopyDesc});
		Temp.SetValue(Blood::Private::V0, ExactDesc->CachedProperty, PropertyBag.GetValue().GetMemory());
		return FBloodValue(MoveTemp(Temp));
	}

	// If access as single value/container1 fails, try access as container2
	{
		FNameBuilder NameBuilder[2];
		Name.ToString(NameBuilder[0]);
		Name.ToString(NameBuilder[1]);
		NameBuilder[0].Append(TEXT("__V0"));
		NameBuilder[1].Append(TEXT("__V1"));
		auto&& DescV0 = PropertyBag.FindPropertyDescByName(FName(NameBuilder[0]));
		auto&& DescV1 = PropertyBag.FindPropertyDescByName(FName(NameBuilder[1]));
		if (DescV0 && DescV1)
		{
			FInstancedPropertyBag Temp;
			TArray<FPropertyBagPropertyDesc> CopyDescs{*DescV0, *DescV1};
			CopyDescs[0].Name = Blood::Private::V0;
			CopyDescs[1].Name = Blood::Private::V1;
			Temp.AddProperties(CopyDescs);
			Temp.SetValue(Blood::Private::V0, DescV0->CachedProperty, PropertyBag.GetValue().GetMemory());
			Temp.SetValue(Blood::Private::V1, DescV1->CachedProperty, PropertyBag.GetValue().GetMemory());
			return FBloodValue(MoveTemp(Temp));
		}
	}

	return {};
}

bool FBloodContainer::Contains(const FName Name) const
{
	return PropertyBag.FindPropertyDescByName(Name) != nullptr;
}

int32 FBloodContainer::Num() const
{
	return PropertyBag.GetNumPropertiesInBag();
}

bool FBloodContainer::IsEmpty() const
{
	return PropertyBag.GetNumPropertiesInBag() == 0;
}