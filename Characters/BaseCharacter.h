#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class MYFIRSTPROJECT_API ABaseCharacter : public ACharacter {
	GENERATED_BODY()

protected:
	/** 왼 주먹 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *LeftFist;
	/** 오른 주먹 콜리전 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *RightFist;
	/** 체력 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UHealthComponent *Health;

protected:
	/** 체크 시 랜덤한 시간 간격을 두고 공격 발동. */
	UPROPERTY(VisibleAnywhere, Category = "Config|Attack")
	bool bUseRandomInterval;
	/** 공격 사이 최소 시간 간격. */
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	float MinAttackInterval;
	/** 공격 사이 최대 시간 간격. */
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	float MaxAttackInterval;

private:
	/** 캐릭터가 데미지를 입을 때 재생하는 이펙트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	class UParticleSystem *DamagedFX;
	/** 다른 캐릭터를 주먹으로 때릴 때 가하는 데미지. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float HitDamage;

public:
	/** 디폴트 생성자. */
	ABaseCharacter();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;
	
public:
	/** 데미지를 입음. */
	virtual float TakeDamage(
			float Damage, struct FDamageEvent const &DamageEvent,
			AController *EventInstigator, AActor *DamageCauser) override;
	/** 체력 회복. */
	void RecoverHealth(float Amount);
	/** 주먹 컴포넌트 콜리전 활성화|비활성화. */
	UFUNCTION(BlueprintCallable)
	void SetFistCollisionEnabled(bool bLeft, bool bEnabled);
	/** 데미지를 입을 때 재생하는 이펙트 반환. */
	FORCEINLINE class UParticleSystem *GetDamagedFX() const { return DamagedFX; }
	/** 자동 공격이면 true 반환. */
	FORCEINLINE bool IsAutoAttack() const { return bAutoAttack; }
	/** 죽은 상태이면 true 반환. */
	FORCEINLINE bool IsDead() const { return bDead; }

protected:
	/** 데미지를 입고 죽었을 때 호출. */
	virtual void Die(AController *EventInstigator, AActor *Causer);
	/** 데미지를 입었지만 죽지 않았을 때 호출. */
	virtual void OnTakeDamage(
			AController *EventInstigator, AActor *Causer, float Damage);
	/** 공격 중이 아니면 공격. */
	void TryAttack();
	/** 공격 중인지 여부에 관계없이 공격. */
	void Attack();
	/** 공격 시작 시 호출되며, 반환한 시간이 경과한 후 공격 종료. */
	virtual float OnStartAttack();
	/** 자동 공격. */
	FORCEINLINE void SetAutoAttack(bool bAuto) { bAutoAttack = bAuto; }
	/** 현재 공격 중. */
	FORCEINLINE bool IsAttacking() const { return bAttacking; }

private:
	/** 공격 종료. */
	void EndAttack();
	/** 주먹 컴포넌트 오버랩 시 이펙트 재생 및 데미지 가함. */
	UFUNCTION()
	void OnFistOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult &SweepResult);

private:
	/** 공격 타이머. */
	FTimerHandle AttackTimer;
	/** 공격 종료 타이머. */
	FTimerHandle EndAttackTimer;
	/** 애니메이션 인스턴스. */
	class UBaseAnim *BaseAnim;
	/** 현재 공격 중. */
	bool bAttacking;
	/** 자동 공격. */
	bool bAutoAttack;
	/** 죽은 상태. */
	bool bDead;
};