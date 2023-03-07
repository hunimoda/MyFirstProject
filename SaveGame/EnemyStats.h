#pragma once

#include "CoreMinimal.h"
#include "EnemyStats.generated.h"

/** 에너미 정보 저장. */
USTRUCT()
struct FEnemyStats {
	GENERATED_BODY()

public:
	/** 트랜스폼 (위치|회전). */
	UPROPERTY()
	FTransform Transform;
	/** 체력. */
	UPROPERTY()
	float Health;
	/** 순찰 지점 리스트. */
	UPROPERTY()
	TArray<FVector> PatrolPoints;
	/** 다음 순찰 지점의 인덱스. */
	UPROPERTY()
	int32 PatrolIndex;
};