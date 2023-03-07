#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosive.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AExplosive : public AActor {
	GENERATED_BODY()

private:
	/** 폭발 뇌관 역할의 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent *TriggerCapsule;
	/** 폭약 모양의 스태틱 메시 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent *StaticMesh;
	/** 메시에 부착된 이펙트 컴포넌트(ex. 로켓포 연기). */
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *AttachedFX;
	/** 데미지를 입히는 범위(오버랩). */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *KillingRange;

private:
	/** 폭발 이펙트. */
	UPROPERTY(EditAnywhere, Category = "Config|FX")
	class UParticleSystem *ExplodeFX;
	/** 폭발 이펙트 스케일. */
	UPROPERTY(EditAnywhere, Category = "Config|FX")
	float ExplodeFXScale;
	/** 액터에 부착된 채로 재생되는 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *AttachedSound;
	/** 폭발 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *ExplodeSound;
	/** 최대 데미지. */
	UPROPERTY(EditAnywhere, Category = "Config|Damage")
	float MaxDamage;
	/** 작용하는 데미지가 감소하기 시작하는 지점의 위치(비율). */
	UPROPERTY(EditAnywhere, Category = "Config|Damage")
	float CuspLocation;
	/** 이펙트가 즉시 사라지는 것을 방지하기 위해 액터 제거를 유예하는 시간. */
	UPROPERTY(EditAnywhere, Category = "Config|Delay")
	float DestroyDelay;
	/** 체크 시 일정 시간 경과 후 자동 폭파. */
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bAutoDestruct;
	/** 자동 폭파까지 걸리는 시간. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float AutoDestructTime;
	
public:
	/** 디폴트 생성자. */
	AExplosive();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

private:
	/** 폭약 폭파. 이펙트 방향을 인자로 전달. */
	void Explode(const FVector &FXDirection);
	/** 폭약 폭파. 이펙트 방향을 자동으로 계산. */
	void Explode();
	/** 폭약 폭파. 힛 이벤트 발생 시 호출. */
	UFUNCTION()
	void Explode(
			UPrimitiveComponent *HitComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, FVector NormalImpulse,
			const FHitResult &Hit);
	/** 이펙트 및 사운드 재생. */
	void SpawnFXAndSound(const FVector &FXDirection);
	/** 살상 반경 내 폰에게 데미지를 입힘. */
	void ApplyDamageToOverlappingActors();
	/** 액터에게 거리를 바탕으로 계산된 데미지를 입힘. */
	void ApplyDamage(class AActor *Actor);
	/** 지연 시간 경과 후 액터 제거. */
	void DestroyAfterDelay();
	/** 액터 제거. */
	void Destroy();

public:
	/** 데미지를 가할 때 원인을 제공한 컨트롤러. */
	class AController *Instigator;

private:
	/** 지연 시간이 경과한 후 액터가 제거되도록 하는 타이머. */
	FTimerHandle DestroyTimer;
	/** 일정 시간이 경과한 후 자동 폭파되도록 하는 타이머. */
	FTimerHandle AutoDestructTimer;
	/** 폭발했는지 여부. */
	bool bExploded;
};