#pragma once

#include "CoreMinimal.h"
#include "../Weapon.h"
#include "Knife.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AKnife : public AWeapon {
	GENERATED_BODY()

private:
	/** 나이프 칼날에 부착될 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent *Blade;
	
public:
	/** 디폴트 생성자. */
	AKnife();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;
	/** 공격. */
	virtual void Attack() override;

private:
	/** 콜리전 컴포넌트 오버랩 활성화. */
	void EnableBladeCollision();
	/** 콜리전 컴포넌트 오버랩 비활성화. */
	void DisableBladeCollision();
	/** 콜리전 컴포넌트 오버랩 시 호출. */
	UFUNCTION()
	void OnBladeBeginOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult &SweepResult);

private:
	/** 콜리전 컴포넌트 오버랩 활성화를 위한 타이머. */
	FTimerHandle EnableTimer;
	/** 콜리전 컴포넌트 오버랩 비활성화를 위한 타이머. */
	FTimerHandle DisableTimer;
};