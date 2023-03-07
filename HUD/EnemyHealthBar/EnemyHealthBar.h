#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UEnemyHealthBar : public UUserWidget {
	GENERATED_BODY()

private:
	/** 체력을 나타내는 위젯. */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *HealthBar;

private:
	/** 위젯 바의 채우기 색. */
	UPROPERTY(EditAnywhere, Category = "Config|Color")
	FLinearColor BarColor;
	/** 위젯 바의 배경 색. */
	UPROPERTY(EditAnywhere, Category = "Config|Color")
	FLinearColor BgColor;

public:
	/** 체력 바가 채워진 비율 설정. */
	void SetRatio(float Ratio);
	/** 체력 바를 보임|숨김. */
	void DisplayHealthBar(bool bDisplay);
};