#pragma once

#include "CoreMinimal.h"
#include "InventoryStats.h"
#include "PlayerStats.generated.h"

/** 플레이어 정보 저장. */
USTRUCT()
struct FPlayerStats {
	GENERATED_BODY()

public:
	/** 트랜스폼 (위치|회전). */
	UPROPERTY()
	FTransform Transform;
	/** 체력. */
	UPROPERTY()
	float Health;
	/** 스태미나. */
	UPROPERTY()
	float Stamina;
	/** 인벤토리. */
	UPROPERTY()
	FInventoryStats InventoryStats;
};