#pragma once

#include "CoreMinimal.h"
#include "../BaseAnim.h"
#include "../../Characters/Enemy/BehaviorStatus.h"
#include "EnemyAnim.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UEnemyAnim : public UBaseAnim {
	GENERATED_BODY()

private:
	/** 에너미의 속력. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	float Speed;
	/** 에너미 활동 상태. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	EBehaviorStatus BehaviorStatus;
	/** 에너미. */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy *Enemy;

public:
	/** 디폴트 생성자. */
	UEnemyAnim();
	/** 게임 시작 시 호출. */
	virtual void NativeBeginPlay() override;
	/** 데이터 수집. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};