#include "SavePoint.h"

#include "../../GameMode/MyGameMode.h"

// 플레이어를 세이브 포인트에 스폰 시 게임을 저장하는 것 방지
void ASavePoint::BeginPlay() {
  Super::BeginPlay();
  bActivated = false;
  GetWorldTimerManager().SetTimer(
      ActivateTimer, this, &ASavePoint::Activate, 1.f);
}

void ASavePoint::OnShooterOverlap(class AShooter *Shooter) {
  if (bActivated) {
    Super::OnShooterOverlap(Shooter);
    GameMode->SaveGame();
  }
  Destroy();
}