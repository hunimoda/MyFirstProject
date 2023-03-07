#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PeriodicalMovementComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTPROJECT_API UPeriodicalMovementComponent : public UActorComponent {
	GENERATED_BODY()

private:
	/** 체크 시 액터가 시작 지점에서 반환점 사이를 왕복함. */
	UPROPERTY(EditAnywhere, Category = "Config|Translation")
	bool bShouldTranslate;
	/** 처음 위치에 대한 반환점의 상대적 위치. */
	UPROPERTY(EditAnywhere, Category = "Config|Translation")
	FVector DeltaPoint;
	/** 시작 지점에서 반환점까지 이동하는 데에 걸리는 시간. */
	UPROPERTY(EditAnywhere, Category = "Config|Translation")
	float TravelTime;
	/** 시작 지점과 반환점에서 머무르는 시간. */
	UPROPERTY(EditAnywhere, Category = "Config|Translation")
	float WaitTime;
	/** 체크 시 회전 효과 발생. */
	UPROPERTY(EditAnywhere, Category = "Config|Rotation")
	bool bShouldRotate;
	/** 회전 주기*/
	UPROPERTY(EditAnywhere, Category = "Config|Rotation")
	float RotatePeriod;

public:	
	/** 디폴트 생성자. */
	UPeriodicalMovementComponent();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:	
	/** 매 프레임마다 호출. */
	virtual void TickComponent(
			float DeltaTime, ELevelTick TickType,
			FActorComponentTickFunction *ThisTickFunction) override;

private:
	/** 왕복 운동. */
	void Translate();
	/** 전체 게임 플레이 시간을 한 주기로 나눈 나머지 시간 반환. */
	float GetModuloTime() const;
	/** 한 사이클 시간 내에서 알파 값 반환. */
	float GetAlpha(float Time) const;
	/** 첫 번째 이동 구간(처음 위치 ~ 반환점)에서 알파 값 반환. */
	float GetAlphaBeforeTravelTime(float Time) const;
	/** 회전 운동. */
	void Rotate(float DeltaTime);

private:
	/** 액터의 처음 위치. */
	FVector InitialPoint;
	/** 왕복 운동 시 반환점. */
	FVector TurningPoint;
};
