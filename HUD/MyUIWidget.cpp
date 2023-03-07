#include "MyUIWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

#include "../GameMode/MyGameMode.h"

// 게임 모드가 블루프린트에서 Construct 단계에 참조되므로 미리 초기화
void UMyUIWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();
  GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
}

// 뷰포트에 추가될 때 UI 모드 진입
void UMyUIWidget::NativeConstruct() {
  Super::NativeConstruct();
  EnableUI();

  // 버튼 클릭 시 위젯을 뷰포트에서 제거
  TArray<UWidget *> Widgets;
  WidgetTree->GetAllWidgets(Widgets);
  for (UWidget *Widget : Widgets) {
    if (UButton *Button = Cast<UButton>(Widget)) {
      Button->OnClicked.AddDynamic(this, &UMyUIWidget::RemoveFromViewport);
    }
  }
}

// 뷰포트에서 제거될 때 UI 모드 중지
void UMyUIWidget::NativeDestruct() {
  Super::NativeDestruct();
  DisableUI();
}

void UMyUIWidget::EnableUI() {
  FInputModeGameAndUI GameAndUI;
  GetOwningPlayer()->SetInputMode(GameAndUI);
  GetOwningPlayer()->SetShowMouseCursor(true);
  UGameplayStatics::SetGamePaused(this, true);
}

void UMyUIWidget::DisableUI() {
  if (!GetOwningPlayer()) return;
  FInputModeGameOnly GameOnly;
  GetOwningPlayer()->SetInputMode(GameOnly);
  GetOwningPlayer()->SetShowMouseCursor(false);
  UGameplayStatics::SetGamePaused(this, false);
}