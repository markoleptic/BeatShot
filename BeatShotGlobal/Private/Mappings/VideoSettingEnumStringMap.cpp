// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/VideoSettingEnumStringMap.h"
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "StreamlineLibraryDLSSG.h"
#include "StreamlineLibraryReflex.h"
#include "Utilities/BSSettingTypes.h"

UVideoSettingEnumStringMap::UVideoSettingEnumStringMap()
{
	PopulateEnumTypes(TSet({
		StaticEnum<EDLSSEnabledMode>(),
		StaticEnum<EStreamlineReflexMode>(),
		StaticEnum<UDLSSMode>(),
		StaticEnum<EStreamlineDLSSGMode>(),
		StaticEnum<ENISEnabledMode>(),
		StaticEnum<UNISMode>()
	}));
}
