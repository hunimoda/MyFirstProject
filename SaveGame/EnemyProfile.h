#pragma once

#include "CoreMinimal.h"
#include "EnemyStats.h"
#include "EnemyProfile.generated.h"

/** 에너미의 블루프린트 클래스 포함. */
USTRUCT()
struct FEnemyProfile {
	GENERATED_BODY()

public:
	/** 에너미의 블루프린트 클래스. */
	UPROPERTY()
	TSubclassOf<class AEnemy> Class;
	/** 에너미의 정보. */
	UPROPERTY()
	FEnemyStats Stats;
};