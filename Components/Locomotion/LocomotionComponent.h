#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LocomotionComponent.generated.h"

/** 부울형 인자를 받는 델리게이트 선언. */
DECLARE_DELEGATE_OneParam(FBoolDelegate, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTPROJECT_API ULocomotionComponent : public UActorComponent {
	GENERATED_BODY()

private:
	/** 걷는 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Locomotion")
	float WalkSpeed;
	/** 달리는 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Locomotion")
	float RunSpeed;
	/** 달리기 위한 최소 스태미나. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Locomotion",
						meta = (AllowPrivateAccess = "true"))
	float MinRunStamina;
	/** 달릴 때 스태미나가 추가로 감소하는 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Locomotion")
	float RunDecreaseRate;
	/** 최소 피치. */
	UPROPERTY(EditAnywhere, Category = "Config|Locomotion")
	float MinPitch;
	/** 최대 피치. */
	UPROPERTY(EditAnywhere, Category = "Config|Locomotion")
	float MaxPitch;
		
public:
	/** 디폴트 생성자. */
	ULocomotionComponent();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:
	/** 매 프레임마다 호출. */
	virtual void TickComponent(
			float DeltaTime, ELevelTick TickType,
			FActorComponentTickFunction *ThisTickFunction) override;
	/** 액션 및 축 매핑. */
	void SetupInputComponent(class UInputComponent *InputComponent);

private:
	/** 점프. */
	void Jump();
	/** 캐릭터가 마우스를 따라서 회전하지 않음. */
	void SetLookAround(bool bLookAround);
	/** 스태미나가 충분하면 달리기로 전환. */
	void TryRun();
	/** 걷기로 전환. */
	void Walk();
	/** 앞뒤로 움직임. */
	void MoveForward(float Scale);
	/** 좌우로 움직임. */
	void MoveRight(float Scale);
	/** 위아래 보기. */
	void LookUp(float Scale);
	/** 좌우로 보기. */
	void LookRight(float Scale);
	/** 달리고 있으면 true 반환. */
	bool IsRunning() const;

private:
	/** 컴포넌트의 오너 플레이어. */
	class AShooter *Shooter;
	/** 플레이어의 컨트롤러. */
	class AController *Controller;
	/** 플레이어의 스태미나. */
	class UHealthComponent *Stamina;
	/** 플레이어의 캐릭터 이동 컴포넌트. */
	class UCharacterMovementComponent *Movement;
};
