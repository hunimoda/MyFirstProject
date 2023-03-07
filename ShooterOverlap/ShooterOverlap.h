#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterOverlap.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AShooterOverlap : public AActor {
	GENERATED_BODY()

protected:
	/** 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *OverlapSphere;
	/** 스태틱 메시 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent *StaticMesh;
	/** 이펙트 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *IdleFX;
	/** 왕복|회전 운동 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UPeriodicalMovementComponent *PeriodicalMovement;

protected:
	/** 체크하면 플레이어와 오버랩 시 사운드 및 이펙트 재생. */
	UPROPERTY(EditAnywhere, Category = "Config|Effects")
	bool bPlayEffectsOnOverlap;

private:
	/** 오버랩 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Effects")
	class USoundBase *OverlapSound;
	/** 오버랩 이펙트. */
	UPROPERTY(EditAnywhere, Category = "Config|Effects")
	class UParticleSystem *OverlapFX;

public:
	/** 디폴트 생성자. 컴포넌트 생성 및 멤버 변수 초기화. */
	AShooterOverlap();
	/** 오버랩 사운드 및 이펙트 재생. 디폴트로 플레이어 오버랩 시 호출하도록 되어 있으나, 무기 아이템과 같이 오버랩 하더라도 이펙트 재생이 유예될 수 있는 경우에는 이 함수를 직접 호출할 수 있음. */
	void PlayOverlapEffects();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;
	/** 플레이어 오버랩 시 호출. */
	virtual void OnShooterOverlap(class AShooter *Shooter);
	
private:
	/** 폰 오버랩 이벤트 델리게이트. */
	UFUNCTION()
	void OnSphereBeginOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, 
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult &SweepResult);

protected:
	/** 게임 모드. */
	class AMyGameMode *GameMode;
};