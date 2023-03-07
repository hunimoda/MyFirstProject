#include "ShooterAnim.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../../Weapon/Weapon.h"
#include "../../Components/Inventory/InventoryComponent.h"
#include "../../Characters/Shooter/Shooter.h"

// 멤버 변수 초기화
UShooterAnim::UShooterAnim() {
  FirearmAttackMontage = nullptr;
  KnifeAttackMontage = nullptr;
  ForwardVelocity = 0.f;
  RightVelocity = 0.f;
  WeaponType = EWeaponType::EWT_NoWeapon;
  bInAir = false;
}

void UShooterAnim::NativeBeginPlay() {
  Super::NativeBeginPlay();
  Shooter = Cast<AShooter>(GetOwningActor());
}

void UShooterAnim::NativeUpdateAnimation(float DeltaSeconds) {
  Super::NativeUpdateAnimation(DeltaSeconds);
  if (Shooter) {
    UpdateVelocity();
    UpdateWeaponType();
    bInAir = Shooter->GetCharacterMovement()->IsFalling();
  }
}

// 사용 중인 무기에 맞는 공격 몽타주 재생
float UShooterAnim::PlayAttackMontage() {
  switch (WeaponType) {
    case EWeaponType::EWT_NoWeapon:
      return Super::PlayAttackMontage();
    case EWeaponType::EWT_Knife:
      return PlayRandomSection(KnifeAttackMontage);
    default:
      return PlayRandomSection(FirearmAttackMontage);
  }
}

// 속도 벡터를 전향|우향 벡터와 내적하여 각 방향의 성분을 구함
// 함수 호출 이전 Shooter 변수가 유효한 값임을 확인해야 함
void UShooterAnim::UpdateVelocity() {
  FVector2D Velocity = FVector2D(Shooter->GetVelocity());
  FVector2D Forward = FVector2D(Shooter->GetActorForwardVector());
  FVector2D Right = FVector2D(Shooter->GetActorRightVector());

  ForwardVelocity = Forward.Dot(Velocity);
  RightVelocity = Right.Dot(Velocity);
}

// 함수 호출 이전 Shooter 변수가 유효한 값임을 확인해야 함
void UShooterAnim::UpdateWeaponType() {
  if (AWeapon *ActiveWeapon = Shooter->GetInventory()->GetActiveWeapon()) {
    WeaponType = ActiveWeapon->GetType();
  } else {
    WeaponType = EWeaponType::EWT_NoWeapon;
  }
}