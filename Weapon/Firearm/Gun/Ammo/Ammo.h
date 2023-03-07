#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Ammo.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AAmmo : public AStaticMeshActor {
	GENERATED_BODY()

private:
	/** 탄피 배출 최대각. 최대각 내 랜덤한 각도로 탄피 배출. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxEjectAngle;
	/** 탄피 배출 시 가할 충격량의 크기. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float EjectImpulse;

public:
	/** 디폴트 생성자. */
	AAmmo();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

private:
	/** 탄피 배출 시 가할 충격량을 랜덤하게 계산하여 반환. */
	FVector GetRandomImpulse() const;
	/** 액터 제거. */
	void Destroy();

private:
	/** 액터가 생성 3초 후에 소멸되도록 하는 타이머. */
	FTimerHandle DestroyTimer;
};