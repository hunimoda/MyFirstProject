#include "ShooterController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../HUD/MyUIWidget.h"

// 정적 멤버 초기화
UHeadsUpDisplay *AShooterController::HeadsUpDisplay = nullptr;

// 멤버 변수 초기화
AShooterController::AShooterController() {
  HUDAsset = ConstructorHelpers::FClassFinder<UUserWidget>(
      TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_HUD.WBP_HUD_C'")).Class;
  PauseGameAsset = ConstructorHelpers::FClassFinder<UUserWidget>(
      TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_PauseMenu.WBP_PauseMenu_C'")).Class;
  GameplayOptionsAsset = ConstructorHelpers::FClassFinder<UUserWidget>(
      TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_GameplayOptions.WBP_GameplayOptions_C'")).Class;
  WinScreenAsset = ConstructorHelpers::FClassFinder<UUserWidget>(
      TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_WinScreen.WBP_WinScreen_C'")).Class;
  LoseScreenAsset = ConstructorHelpers::FClassFinder<UUserWidget>(
      TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_LoseScreen.WBP_LoseScreen_C'")).Class;
}

// PIE 환경에서 게임 플레이 시 정적 멤버가 초기화 되지 않는 문제 해결
void AShooterController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Super::EndPlay(EndPlayReason);
  HeadsUpDisplay = nullptr;
}

void AShooterController::SetupInputComponent() {
  Super::SetupInputComponent();

  // 게임 일시정지 창 보이기|숨기기
  InputComponent->BindAction(
      TEXT("TogglePauseGame"), IE_Pressed, this,
      &AShooterController::TogglePauseGame).bExecuteWhenPaused = true;

	// 미니맵 크기 변경 (N)
	InputComponent->BindAction(
      TEXT("ToggleMinimapSize"), IE_Pressed, this,
			&AShooterController::ToggleMinimapSize);
}

void AShooterController::StartHUD() {
  HeadsUpDisplay = DisplayWidget<UHeadsUpDisplay>(HUDAsset);
  HeadsUpDisplay->UpdatePlayerLives();
  HeadsUpDisplay->UpdateEnemyCount();
  HeadsUpDisplay->UpdateWeapon();
}

void AShooterController::DisplayGameplayOptions() {
  DisplayWidget(GameplayOptionsAsset);
}

void AShooterController::DisplayWinScreen() { DisplayWidget(WinScreenAsset); }

void AShooterController::DisplayLoseScreen() { DisplayWidget(LoseScreenAsset); }

void AShooterController::DisplayLog(const FString &Message) {
  if (HeadsUpDisplay) HeadsUpDisplay->DisplayLogMessage(Message);
}

template<typename WidgetT>
WidgetT *AShooterController::DisplayWidget(TSubclassOf<WidgetT> WidgetAsset) {
  WidgetT *Widget = CreateWidget<WidgetT>(this, WidgetAsset);
  Widget->AddToViewport();
  return Widget;
}

// (1) 뷰포트에 추가된 위젯을 추적 (HUD 제외)
// (2) 추가된 위젯이 있고 일시정지 창이면 뷰포트에서 제거
// (3) 추가된 위젯이 없으면 일시정지 창 보임
void AShooterController::TogglePauseGame() {
  TArray<UUserWidget *> FoundWidgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
      this, FoundWidgets, UMyUIWidget::StaticClass());  // (1)

  if (FoundWidgets.Num()) {
    UMyUIWidget *FoundWidget = Cast<UMyUIWidget>(FoundWidgets[0]);
    if (FoundWidget->IsA(PauseGameAsset)) {
      FoundWidget->RemoveFromViewport();  // (2)
    }
  } else {
    DisplayWidget(PauseGameAsset); // (3)
  }
}

void AShooterController::ToggleMinimapSize() {
  if (HeadsUpDisplay) HeadsUpDisplay->ToggleMinimapSize();
}