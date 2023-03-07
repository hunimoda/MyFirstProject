#pragma once

#include "CoreMinimal.h"
#include "../../ShooterOverlap/ShooterOverlap.h"
#include "SavePoint.generated.h"

UCLASS()
class MYFIRSTPROJECT_API ASavePoint : public AShooterOverlap {
	GENERATED_BODY()

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;
	/** 플레이어 오버랩 시 호출. */
	virtual void OnShooterOverlap(class AShooter *Shooter);

private:
	/** 세이브 포인트가 플레이어와 오버랩 시 게임을 저장할 수 있도록 함. */
	FORCEINLINE void Activate() { bActivated = true; }

private:
	/** 세이브 포인트를 게임 시작 1초 후 활성화 하도록 하는 타이머. */
	FTimerHandle ActivateTimer;
	/** 플레이어와 오버랩 시 게임을 저장. */
	bool bActivated;
};