#pragma once

#include "CoreMinimal.h"
#include "../../ShooterOverlap/ShooterOverlap.h"
#include "Portal.generated.h"

UCLASS()
class MYFIRSTPROJECT_API APortal : public AShooterOverlap {
	GENERATED_BODY()

private:
	/** 다음 레벨 이름. */
	UPROPERTY(EditAnywhere, Category = "Config")
	FString NextLevel;
	/** 체크하면 마지막 포탈이 됨. 오버랩 시 모든 레벨을 클리어 하고 게임 승리. */
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bEndOfGame;

public:
	/** 디폴트 생성자. */
	APortal();
	/** 포탈을 활성화|비활성화 시킴. */
	void SetActivated(bool bActivate);

protected:
	/** 플레이어 오버랩 시 호출. */
	virtual void OnShooterOverlap(class AShooter *Shooter) override;
};