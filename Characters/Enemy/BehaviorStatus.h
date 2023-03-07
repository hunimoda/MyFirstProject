#pragma once

#include "BehaviorStatus.generated.h"

/** 에너미 상태. */
UENUM(BlueprintType)
enum class EBehaviorStatus : uint8 {
	/** 게임 시작 전. */
	EBS_Spawned	UMETA(DisplayName = "스폰"),

	/** 게임 시작 후. */
	EBS_Idle 		UMETA(DisplayName = "휴식"),
	EBS_Patrol 	UMETA(DisplayName = "순찰"),
	EBS_Chase 	UMETA(DisplayName = "추격"),
	EBS_Attack 	UMETA(DisplayName = "공격"),

	/** 죽어서 움직일 수 없는 상태. */
	EBS_Dead		UMETA(DisplayName = "죽음"),
	/** 죽은 후 땅으로 가라앉는 상태. */
	EBS_Sink		UMETA(DisplayName = "하강")
};