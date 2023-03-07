#include "Item.h"
#include "Particles/ParticleSystemComponent.h"

#include "../../Characters/Shooter/Shooter.h"
#include "../../Components/Inventory/InventoryComponent.h"

AItem::AItem() {
  IdleFX->SetTemplate(
      ConstructorHelpers::FObjectFinder<UParticleSystem>(
          TEXT("ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Treasure/Gold_Bag/P_GoldSparkle_Bag_Loop.P_GoldSparkle_Bag_Loop'")).Object);

  // 멤버 변수 초기화
  ItemType = EItemType::EIT_Ammo;
  TargetWeapon = nullptr;
  AmmoCount = 0;
  WeaponBP = nullptr;
  PotionEffect = EPotionEffect::EPE_RecoverHealth;
  EffectAmount = 0.f;
}

void AItem::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  switch (ItemType) {
    case EItemType::EIT_Ammo:   EquipAmmo(Shooter);       break;
    case EItemType::EIT_Weapon: TryEquipWeapon(Shooter);  break;
    case EItemType::EIT_Potion: ApplyPotion(Shooter);     break;
  }
}

void AItem::EquipAmmo(class AShooter *Shooter) {
  Shooter->GetInventory()->EquipAmmo(TargetWeapon, AmmoCount);
  Destroy();
}

void AItem::TryEquipWeapon(class AShooter *Shooter) {
  Shooter->GetInventory()->TryEquipWeapon(this);
}

void AItem::ApplyPotion(class AShooter *Shooter) {
  switch (PotionEffect) {
    case EPotionEffect::EPE_RecoverHealth:
      Shooter->RecoverHealth(EffectAmount);  // 체력 회복
      break;
    case EPotionEffect::EPE_RecoverStamina:
      Shooter->RecoverStamina(EffectAmount);  // 스태미나 회복
      break;
  }
  Destroy();
}