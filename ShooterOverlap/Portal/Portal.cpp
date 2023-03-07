#include "Portal.h"
#include "Components/SphereComponent.h"

#include "../../GameMode/MyGameMode.h"

// 멤버 변수 초기화
APortal::APortal() {
  NextLevel = TEXT("");
  bEndOfGame = false;
}
 
void APortal::SetActivated(bool bActivate) {
  OverlapSphere->SetGenerateOverlapEvents(bActivate);
  SetActorHiddenInGame(!bActivate);
}

void APortal::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  if (bEndOfGame) {
    GameMode->ClearGame();
  } else {
    GameMode->ClearLevel(NextLevel);
  }
}