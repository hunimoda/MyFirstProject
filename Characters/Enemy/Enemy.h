#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "BehaviorStatus.h"
#include "Enemy.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AEnemy : public ABaseCharacter {
	GENERATED_BODY()

private:
	/** 에너미 머리 위에 표시되는 체력 위젯 컴포넌트. */
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent *HealthBar;
	/** 적이 플레이어 발견 시 추격을 시작할 수 있는 범위. */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *ChaseSphere;
	/** 적이 플레이어 발견 시 공격할 수 있는 범위. */
	UPROPERTY(VisibleAnywhere)
	class USphereComponent *AttackSphere;
	
private:
	/** 순찰 지점 리스트. */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (MakeEditWidget = "true"))
	TArray<FVector> PatrolPoints;
	/** 다음 순찰 지점 이동 전까지 휴식 시간. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float IdleTime;
	/** 죽은 후 하강 속도. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float SinkSpeed;
	/** 에너미 활동 상태. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats",
						meta = (AllowPrivateAccess = "true"))
	EBehaviorStatus BehaviorStatus;
	/** 다음 순찰 지점의 인덱스. */
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	int32 PatrolIndex;

public:
	/** 디폴트 생성자. */
	AEnemy();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:
	/** 매 프레임마다 호출. */
	virtual void Tick(float DeltaSeconds) override;
	/** 에너미 정보 로드. */
	void Load(const struct FEnemyStats &Stats);
	/** 에너미 정보 저장. */
	void Save(struct FEnemyStats &Stats);
	/** 에너미 상태 반환. */
	FORCEINLINE EBehaviorStatus GetBehaviorStatus() const { return BehaviorStatus; }

protected:
	/** 에너미가 죽었을 때 호출. */
	virtual void Die(AController *EventInstigator, AActor *Causer) override;
	/** 데미지를 입었지만 죽지는 않았을 때 호출. */
	virtual void OnTakeDamage(AController *EventInstigator,
														AActor *Causer, float Damage) override;

private:
	/** 상태 전환. */
	void TransitTo(EBehaviorStatus NewBehaviorStatus);
	/** 상태 전환. */
	void TransitTo(struct FAIRequestID RequestID,
								 const struct FPathFollowingResult &Result,
								 EBehaviorStatus NewBehaviorStatus);
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 휴식 상태로 전환. */
	void TransitToIdle();
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 순찰 상태로 전환. */
	void TransitToPatrol();
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 추격 상태로 전환. */
	void TransitToChase();
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 공격 상태로 전환. */
	void TransitToAttack();
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 죽은 상태로 전환. */
	void TransitToDead();
	/** TransitTo 내부에서만 호출하고 직접 호출하지 말 것: 하강 상태로 전환. */
	void TransitToSink();
	
private:
	/** 에너미를 월드에서 삭제. */
	void DestroyEnemy();
	/** 순찰 지점을 로컬 좌표에서 월드 좌표로 변환. */
	void ConvertPatrolPointsToWorld();
	/** 일정 시간 동안 체력 바를 보임. */
	void DisplayHealthBarForTime();
	/** DisplayHealthBarForTime 함수를 호출하고 5초가 경과한 후에 호출. */
	void OnDisplayHealthBarTimeout();
	/** 체력 바를 보이거나 숨김. */
	void DisplayHealthBar(bool bDisplay);
	/** 플레이어가 추격 범위 내에 들어왔을 때 호출. */
	UFUNCTION()
	void OnChaseSphereBeginOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult &SweepResult);
	/** 플레이어가 추격 범위를 벗어났을 때 호출. */
	UFUNCTION()
	void OnChaseSphereEndOverlap(
			UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
			UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
	/** 프레임 시간에 비례하는 만큼 액터를 하강시킴. */
	void Sink(float DeltaSeconds);
	/** 에너미 활동 상태와 관련된 정보를 모두 초기화 함. */
	void InitializeStateVariables();

private:
	/** 일정 시간 휴식 후 순찰하도록 하는 타이머. */
	FTimerHandle IdleToPatrolTimer;
	/** 죽은 지 10초 후 하강하도록 하는 타이머. */
	FTimerHandle DeathToSinkTimer;
	/** 하강 5초 후 액터를 제거하도록 하는 타이머. */
	FTimerHandle SinkToDestroyTimer;
	/** 체력 바를 보이고 일정 시간이 경과한 후 위젯을 숨기기 위한 타이머. */
	FTimerHandle HealthBarTimer;

private:
	/** 게임 모드. */
	class AMyGameMode *GameMode;
	/** AI 컨트롤러. */
	class AAIController *AIController;
	/** AI 컨트롤러의 PathFollowingComponent 서브 오브젝트. */
	class UPathFollowingComponent *PathFollowing;
	/** 플레이어. */
	class AShooter *Shooter;
	/** 체력 바 위젯. */
	class UEnemyHealthBar *WHealthBar;
};