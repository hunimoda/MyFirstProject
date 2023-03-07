#pragma once

#include "CoreMinimal.h"
#include "../Firearm.h"
#include "Launcher.generated.h"

UCLASS()
class MYFIRSTPROJECT_API ALauncher : public AFirearm {
	GENERATED_BODY()

private:
  /** 발사되는 폭약 블루프린트 클래스. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class AExplosive> ExplosiveBP;

public:
  /** 디폴트 생성자. */
	ALauncher();

protected:
	/** 공격. */
	virtual void Attack() override;
};