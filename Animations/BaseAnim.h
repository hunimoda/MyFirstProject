#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnim.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UBaseAnim : public UAnimInstance {
	GENERATED_BODY()

private:
  /** 기본 공격 몽타주. */
	UPROPERTY(EditAnywhere, Category = "Config|Montage")
	class UAnimMontage *AttackMontage;
  /** 죽음 몽타주. */
	UPROPERTY(EditAnywhere, Category = "Config|Montage")
	class UAnimMontage *DeathMontage;

public:
  /** 디폴트 생성자. */
	UBaseAnim();
  /** 기본 공격 몽타주 재생. */
	virtual float PlayAttackMontage();
  /** 죽음 몽타주 재생. */
	float PlayDeathMontage();

protected:
  /** 몽타주에서 랜덤으로 선택된 섹션을 재생. */
  float PlayRandomSection(class UAnimMontage *AnimMontage);

private:
	/** "Random_" 접두사가 붙은 섹션들 중에서 임의 선택된 섹션의 인덱스를 반환. */
	int32 GetRandomIndex(class UAnimMontage *AnimMontage) const;
};