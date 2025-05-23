﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Field.h"
#include "Containers/Map.h"

struct FBloodValue;

namespace Blood
{
	using FFPropertyReadFunc = TFunction<FBloodValue(const FProperty* ValueProp, const uint8* ValuePtr)>;

	class FPrecomputedMaps
	{
		FPrecomputedMaps();

	public:
		static const FPrecomputedMaps& Get();

		TMap<FFieldClass*, FFPropertyReadFunc> ReaderMap;
	};
}
