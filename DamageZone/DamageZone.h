#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageZone.generated.h"

UCLASS()
class MYFIRSTPROJECT_API ADamageZone : public AActor {
	GENERATED_BODY()

private:
	/** 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent *TriggerBox;
	/** 이펙트 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *IdleFX;
	/** 왕복|회전 운동 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UPeriodicalMovementComponent *PeriodicalMovement;

private:
	/** 초당 가하는 데미지 양. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float DamageRate;

public:
	/** 디폴트 생성자. */
	ADamageZone();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:
	/** 매 프레임 호출. */
	virtual void Tick(float DeltaSeconds) override;

private:
	/** 오버랩 시작 델리게이트. */
	UFUNCTION()
	void OnBeginOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult &SweepResult);
	/** 오버랩 끝 델리게이트. */
	UFUNCTION()
	void OnEndOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

private:
	/** 데미지를 가할 폰 리스트. */
	TArray<class APawn *> PawnsToDamage;
};