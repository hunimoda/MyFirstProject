#include "Launcher.h"
#include "Kismet/KismetMathLibrary.h"

#include "../../../Characters/Shooter/Shooter.h"
#include "../../../Explosive/Explosive.h"

// 멤버 변수 초기화
ALauncher::ALauncher() {
  Type = EWeaponType::EWT_Launcher;
  AttackWaitTime = 5.f;
  MaxLoadAmmos = 1;
  ExplosiveBP = nullptr;
}

// 카메라 방향으로 폭약의 머리 부분이 오도록 소켓 위치에 폭약 생성
void ALauncher::Attack() {
  Super::Attack();
  FVector SocketLocation = Mesh->GetSocketLocation(TEXT("AmmoSocket"));
  FRotator Rotation = UKismetMathLibrary::MakeRotFromZ(
      Shooter->GetControlRotation().Vector());
  AExplosive *Explosive = GetWorld()->SpawnActor<
      AExplosive>(ExplosiveBP, SocketLocation, Rotation);
  Explosive->Instigator = Shooter->GetController();
}