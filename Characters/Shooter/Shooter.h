#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Shooter.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AShooter : public ABaseCharacter {
	GENERATED_BODY()

private:
	/** 스프링암 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent *CameraBoom;
	/** 카메라 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent *FollowCamera;
	/** 스태미나 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UHealthComponent *Stamina;
	/** 무기 인벤토리 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UInventoryComponent *Inventory;
	/** 이동 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class ULocomotionComponent *Locomotion;

private:
	/** 평상 시 스태미나 감소 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Stamina")
	float NormalDecreaseRate;
	/** 스프링암 오프셋 보간 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Misc")
	float OffsetInterpSpeed;
	/** 스프링암 오프셋 크기. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float PlayerOffset;
	/** 스태미나가 가득 찼을 때 체력 회복 속도. */
	UPROPERTY(EditAnywhere, Category = "Config|Misc")
	float MaxRecoveryRate;

public:
	/** 디폴트 생성자. */
	AShooter();
	
protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:	
	/** 매 프레임마다 호출. */
	virtual void Tick (float DeltaSeconds) override;
	/** 액션 및 축 매핑. */
	virtual void SetupPlayerInputComponent(
			class UInputComponent *PlayerInputComponent) override;
	/** 캐릭터 정보 로드. */
	void Load(const struct FPlayerStats &PlayerStats, bool bPlayerStart);
	/** 캐릭터 정보 저장. */
	void Save(struct FPlayerStats &PlayerStats);
	/** 스태미나 회복. */
	void RecoverStamina(float Amount);
	/** 플레이어 위치를 화면 중앙에서 오프셋 시킴. */
	void OffsetPlayer(bool bOffset);
	/** 인벤토리 반환. */
	FORCEINLINE class UInventoryComponent *GetInventory() const { return Inventory; }
	/** 스태미나 컴포넌트 반환. */
	FORCEINLINE class UHealthComponent *GetStaminaComponent() const { return Stamina; }

protected:
	/** 죽었을 때 호출. */
	virtual void Die(AController *EventInstigator, AActor *Causer) override;
	/** 공격 시작 시 호출. */
	virtual float OnStartAttack() override;

private:
	/** 왼쪽 마우스를 누를 때 호출. */
	void OnLMBPressed();
	/** 왼쪽 마우스를 뗄 때 호출. */
	void OnLMBReleased();
	/** 스프링암 오프셋 보간. */
	void InterpSpringArmOffset(float DeltaSeconds);
	/** 틱 함수를 비활성화 해야 할 때 true 반환. */
	bool ShouldDisableTick() const;
	/** HUD에 체력 업데이트. */
	void UpdateHealth(float Ratio);
	/** HUD에 스태미나 업데이트. */
	void UpdateStamina(float Ratio);
	
private:
	/** 게임 모드. */
	class AMyGameMode *GameMode;
	/** 스프링암의 소켓 오프셋. 카메라뷰 변경 시 Tick 내부에서 이 값을 목표로 보간. */
	float TargetOffset;
};