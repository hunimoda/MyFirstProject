#pragma once

#include "CoreMinimal.h"
#include "../BaseAnim.h"
#include "../../Weapon/WeaponType.h"
#include "ShooterAnim.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UShooterAnim : public UBaseAnim {
	GENERATED_BODY()

private:
	/** 화기류 공격 몽타주. */
	UPROPERTY(EditAnywhere, Category = "Config|Montage")
	class UAnimMontage *FirearmAttackMontage;
	/** 나이프 공격 몽타주. */
	UPROPERTY(EditAnywhere, Category = "Config|Montage")
	class UAnimMontage *KnifeAttackMontage;

private:
	/** 전향 속도. 후향 이동 중일 경우 음수. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	float ForwardVelocity;
	/** 우향 속도. 좌향 이동 중일 경우 음수. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	float RightVelocity;
	/** 사용 중인 무기 종류. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	/** 플레이어가 공중에 떠있음(점프 중). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
						meta = (AllowPrivateAccess = "true"))
	bool bInAir;

public:
	/** 디폴트 생성자. */
	UShooterAnim();
	/** 게임 시작 시 호출. */
	virtual void NativeBeginPlay() override;
	/** 데이터 수집. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	/** 공격 몽타주 재생. */
	virtual float PlayAttackMontage() override;

private:
	/** 전향|우향 속도 업데이트. */
	void UpdateVelocity();
	/** 사용 중인 무기 업데이트. */
	void UpdateWeaponType();

private:
	/** 플레이어. */
	class AShooter *Shooter;
};