#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUIWidget.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UMyUIWidget : public UUserWidget {
	GENERATED_BODY()

private:
	/** 게임 모드. */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AMyGameMode *GameMode;

protected:
	/** 위젯이 생성될 때 호출. */
	virtual void NativeOnInitialized() override;
	/** 위젯이 뷰포트에 추가될 때마다 호출. */
	virtual void NativeConstruct() override;
	/** 위젯이 뷰포트에서 제거될 때마다 호출. */
	virtual void NativeDestruct() override;

private:
	/** UI 모드 진입. */
	void EnableUI();
	/** UI 모드 중지. */
	void DisableUI();
};