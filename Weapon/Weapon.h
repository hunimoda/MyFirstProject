#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AWeapon : public AActor {
	GENERATED_BODY()

protected:
	/** 루트 씬 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USceneComponent *RootScene;
	/** 스켈레탈 메시 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent *Mesh;

protected:
	/** 에너미 외의 액터와 충돌 시 보일 이펙트. */
	UPROPERTY(EditAnywhere, Category = "Config|FX")
	class UParticleSystem *HitFX;
	/** 공격 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *AttackSound;
	/** 무기 사용(장착) 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *RaiseSound;
	/** 무기 비활성화 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *LowerSound;
	/** 무기 종류. */
	UPROPERTY(EditAnywhere, Category = "Config")
	EWeaponType Type;
	/** 로그 출력 등에 보일 무기 이름. */
	UPROPERTY(EditAnywhere, Category = "Config")
	FString WeaponName;
	/** 공격 피해량. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float Damage;
	/** 공격 대기 시간. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float AttackWaitTime;

private:
	/** 연사 전환 가능 여부. */
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCanToggleAuto;

public:
	/** 디폴트 생성자. */
	AWeapon();
	/** 무기의 주인 설정. */
	virtual void SetOwner(AActor *NewOwner) override;
	/** 공격 시도. */
	void TryAttack();
	/** 연사 전환 시도. */
	void TryToggleAuto();
	/** 자동 사격이면 true 반환. */
	FORCEINLINE bool IsAutoAttack() const { return bAutoAttack; }
	/** 무기 사용 사운드 반환. */
	FORCEINLINE class USoundBase *GetRaiseSound() const { return RaiseSound; }
	/** 무기 비활성화 사운드 반환. */
	FORCEINLINE class USoundBase *GetLowerSound() const { return LowerSound; }
	/** 무기 종류 반환. */
	FORCEINLINE EWeaponType GetType() const { return Type; }
	/** 무기 이름 반환. 일반적으로 무기 종류와 다름. */
	FORCEINLINE FString GetWeaponName() const { return WeaponName; }

protected:
	/** 공격 시도 시 실제로 공격을 수행하는 조건을 정의. */
	virtual bool CanAttack();
	/** 공격 시도 시 CanAttack이 true를 반환하는 상황에 호출. */
	virtual void Attack();
	/** 공격 시도 시 CanAttack이 false를 반환하는 상황에 호출. */
	virtual void OnAttackFailure();
	/** 액터에게 Damage(공격 피해량)만큼의 데미지를 입힘. */
	void ApplyDamage(AActor *DamagedActor);

private:
	/** 공격 종료 시 호출. */
	void OnAttackEnd();

protected:
	/** 플레이어. */
	class AShooter *Shooter;
	/** 자동 공격. */
	bool bAutoAttack;
	
private:
	/** 실제로 공격을 감행했을 때 공격을 끝낼 수 있도록 하는 타이머. */
	FTimerHandle EndAttackTimer;
	/** 공격 중. 공격하고 있는 도중에 다시 공격할 수 없도록 함. */
	bool bAttacking;
};