#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"

/** 정적 메서드를 간단히 호출하기 위한 매크로 정의. */
#define LOG(Format, ...) (\
		AShooterController::DisplayLog(\
				FString::Printf(TEXT(Format), ##__VA_ARGS__)))
#define HUD (AShooterController::GetHUD())

UCLASS()
class MYFIRSTPROJECT_API AShooterController : public APlayerController {
	GENERATED_BODY()

private:
	/** HUD 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class UHeadsUpDisplay> HUDAsset;
	/** 게임 일시정지 창 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class UMyUIWidget> PauseGameAsset;
	/** 게임 옵션 창 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class UMyUIWidget> GameplayOptionsAsset;
	/** 게임 승리 스크린 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class UMyUIWidget> WinScreenAsset;
	/** 게임 패배 스크린 블루프린트. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<class UMyUIWidget> LoseScreenAsset;

public:
	/** 디폴트 생성자. */
	AShooterController();
	/** 게임 종료 시 호출. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	/** 액션 바인딩. */
	virtual void SetupInputComponent() override;

public:
	/** HUD를 생성하여 업데이트 된 정보로 표시. */
	void StartHUD();
	/** 플레이 옵션 팝업을 표시. */
	void DisplayGameplayOptions();
	/** 게임 승리 스크린 표시. */
	void DisplayWinScreen();
	/** 게임 패배 스크린 표시. */
	void DisplayLoseScreen();
	/** 잠깐 동안 로그 메시지 출력. */
	static void DisplayLog(const FString &Message);
	/** HUD 포인터 반환. */
	FORCEINLINE static class UHeadsUpDisplay *GetHUD() { return HeadsUpDisplay; }

private:
	/** 위젯을 생성하여 뷰포트에 추가. 생성된 위젯의 포인터를 반환. */
	template<typename WidgetT = class UMyUIWidget>
	WidgetT *DisplayWidget(TSubclassOf<WidgetT> WidgetAsset);
	/** 게임 일시정지 토글. */
	void TogglePauseGame();
	/** 미니맵 사이즈 토글. */
	void ToggleMinimapSize();

private:
	/** HUD. */
	static class UHeadsUpDisplay *HeadsUpDisplay;
};