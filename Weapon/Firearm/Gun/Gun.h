#pragma once

#include "CoreMinimal.h"
#include "../Firearm.h"
#include "Gun.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AGun : public AFirearm {
	GENERATED_BODY()

private:
	/** 트레이스 거리. 이 거리 이상의 물체는 맞출 수 없음. */
	UPROPERTY(EditAnywhere, Category = "Config")
	float TraceLength;
	/** 배출되는 탄피 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class AAmmo> AmmoClass;

public:
	/** 디폴트 생성자. */
	AGun();

protected:
	/** 공격. */
	virtual void Attack() override;

private:
	/** 총알 채널의 라인 트레이스 수행. 힛 결과를 매개변수와 참값으로 반환. */
	bool LineTrace(FHitResult &OutHit);
	/** 힛 방향에 맞게 이팩트 생성. */
	void SpawnHitFX(class UParticleSystem *FX, const FHitResult &HitResult);
	/** 탄피 배출. */
	void EjectAmmo();
};