#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/** float 인자를 전달받는 델리게이트 선언. */
DECLARE_DELEGATE_OneParam(FFloatDelegate, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFIRSTPROJECT_API UHealthComponent : public UActorComponent {
	GENERATED_BODY()

private:
	/** 체력의 최댓값. */
	UPROPERTY(EditAnywhere, Category = "Config|Health")
	float MaxValue;
	/** 체력의 현재 값. */
	UPROPERTY(EditAnywhere, Category = "Config|Health")
	float Value;

public:	
	/** 디폴트 생성자. */
	UHealthComponent();

public:
	/** 체력 증가. */
	void Increase(float Amount);
	/** 체력 감소. */
	void Decrease(float Amount);
	/** 체력이 바닥남. */
	bool IsZero() const;
	/** 체력의 현재 값 설정. */
	void SetValue(float NewValue);
	/** 체력의 현재 값 반환. */
	FORCEINLINE float GetValue() const { return Value; }
	/** 체력의 최댓값에 대한 현재 값의 비율 반환. */
	FORCEINLINE float GetRatio() const { return Value / MaxValue; }

public:
	/** 체력 변화 시 호출할 델리게이트. */
	FFloatDelegate OnChangeValue;
};