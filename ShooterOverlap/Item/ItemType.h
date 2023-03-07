#pragma once

#include "CoreMinimal.h"
#include "ItemType.generated.h"

/** 아이템의 종류 열거형. */
UENUM(BlueprintType)
enum class EItemType : uint8 {
	EIT_Ammo		UMETA(DisplayName = "총알"),
	EIT_Weapon	UMETA(DisplayName = "무기"),
	EIT_Potion	UMETA(DisplayName = "물약")
};

/** 물약의 효과 열거형. */
UENUM()
enum class EPotionEffect : uint8 {
	EPE_RecoverHealth		UMETA(DisplayName = "체력 회복"),
	EPE_RecoverStamina	UMETA(DisplayName = "스태미나 회복"),
};