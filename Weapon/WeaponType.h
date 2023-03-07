#pragma once

#include "WeaponType.generated.h"

/** 무기 종류. */
UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	EWT_PrimaryGun		= 0U	UMETA(DisplayName = "Primary Gun"),
	EWT_SecondaryGun	= 1U	UMETA(DisplayName = "Secondary Gun"),
	EWT_Pistol				= 2U	UMETA(DisplayName = "Pistol"),
  EWT_Knife					= 3U	UMETA(DisplayName = "Knife"),
  EWT_Launcher			= 4U	UMETA(DisplayName = "Launcher"),

	/** 무장하지 않은 상태. */
	EWT_NoWeapon						UMETA(DisplayName = "NoWeapon")
};