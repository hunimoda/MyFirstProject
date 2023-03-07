#pragma once

#include "CoreMinimal.h"
#include "../Weapon/WeaponType.h"
#include "InventoryStats.generated.h"

/** 플레이어의 무기 인벤토리 정보 저장. */
USTRUCT()
struct FInventoryStats {
	GENERATED_BODY()

public:
	/** 보유 무기 리스트. */
	UPROPERTY()
	TMap<EWeaponType, TSubclassOf<class AWeapon>> Weapons;
	/** 화기별 장전되지 않은 총알 수. */
	UPROPERTY()
	TMap<TSubclassOf<class AFirearm>, uint32> UnloadedAmmos;
	/** 화기별 장전된 총알의 수. */
	UPROPERTY()
	TMap<TSubclassOf<class AFirearm>, uint32> LoadedAmmos;
	/** 사용 중인 무기 타입. 무장하고 있지 않으면 NoWeapon. */
	UPROPERTY()
	EWeaponType ActiveWeaponType;
};