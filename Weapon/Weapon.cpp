#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

#include "../Controller/ShooterController.h"
#include "../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../Characters/Shooter/Shooter.h"
#include "../Animations/ShooterAnim/ShooterAnim.h"

AWeapon::AWeapon() {
  // 루트 씬 컴포넌트 생성
  RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
  SetRootComponent(RootScene);

  // 스켈레탈 메시 생성
  Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(RootScene);

  // 멤버 변수 초기화
  HitFX = nullptr;
  AttackSound = nullptr;
  RaiseSound = nullptr;
  LowerSound = nullptr;
  Type = EWeaponType::EWT_NoWeapon;
  WeaponName = TEXT("");
  Damage = 0.f;
  AttackWaitTime = 1.f;
  bCanToggleAuto = false;
  Shooter = nullptr;
  bAutoAttack = false;
  bAttacking = false;
}

void AWeapon::SetOwner(AActor *NewOwner) {
  Super::SetOwner(NewOwner);
  Shooter = Cast<AShooter>(NewOwner);
}

void AWeapon::TryAttack() {
  if (CanAttack()) {
    Attack();
  } else {
    OnAttackFailure();
  }
}

void AWeapon::TryToggleAuto() {
  if (bCanToggleAuto) {
    bAutoAttack ^= true;
    LOG("사격 모드 변경: %s", bAutoAttack ? TEXT("자동") : TEXT("반자동"));
    if (HUD) HUD->DisplayAuto(bAutoAttack);
  }
}

bool AWeapon::CanAttack() { return !bAttacking; }

void AWeapon::Attack() {
  // 공격하는 동안 bAttacking을 true로 만듦
  bAttacking = true;
  GetWorldTimerManager().SetTimer(
      EndAttackTimer, this, &AWeapon::OnAttackEnd, AttackWaitTime);

  // 애니메이션|사운드 재생
  UShooterAnim *ShooterAnim =
      Cast<UShooterAnim>(Shooter->GetMesh()->GetAnimInstance());
  ShooterAnim->PlayAttackMontage();
  UGameplayStatics::PlaySound2D(this, AttackSound);
}

void AWeapon::OnAttackFailure() {}

void AWeapon::ApplyDamage(AActor *DamagedActor) {
  AController *EventInstigator = Shooter->GetController();
  UGameplayStatics::ApplyDamage(
      DamagedActor, Damage, EventInstigator, this, UDamageType::StaticClass());
}

// 무기가 연사 모드이고 마우스를 계속 누른 상태라면 다시 공격 시도
void AWeapon::OnAttackEnd() {
  bAttacking = false;
  if (Shooter->IsAutoAttack() && bAutoAttack) {
    TryAttack();
  }
}