#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AccelMovementComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTPROJECT_API UAccelMovementComponent : public UActorComponent {
	GENERATED_BODY()

private:
	/** 가속도의 크기. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float Acceleration;
	/** 최대 속력. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxSpeed;

public:	
	/** 디폴트 생성자. */
	UAccelMovementComponent();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:	
	/** 매 프레임마다 호출. */
	virtual void TickComponent(
			float DeltaTime, ELevelTick TickType,
			FActorComponentTickFunction *ThisTickFunction) override;

private:
	/** 오너 액터의 루트 컴포넌트. */
	class UPrimitiveComponent *RootComponent;
	/** 오너 액터의 상방 벡터. */
	FVector UpVector;
};