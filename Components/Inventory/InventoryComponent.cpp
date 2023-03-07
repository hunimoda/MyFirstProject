#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../../Weapon/Weapon.h"
#include "../../ShooterOverlap/Item/Item.h"
#include "../../Weapon/Firearm/Firearm.h"
#include "../../Controller/ShooterController.h"
#include "../../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../../SaveGame/MySaveGame.h"
#include "../../Characters/Shooter/Shooter.h"

UInventoryComponent::UInventoryComponent() {
  bWantsInitializeComponent = true;

  // 무기 리스트 초기화
  Weapons.Add(EWeaponType::EWT_PrimaryGun,    nullptr);
  Weapons.Add(EWeaponType::EWT_SecondaryGun,  nullptr);
  Weapons.Add(EWeaponType::EWT_Pistol,        nullptr);
  Weapons.Add(EWeaponType::EWT_Knife,         nullptr);
  Weapons.Add(EWeaponType::EWT_Launcher,      nullptr);

  // 멤버 변수 초기화
  OverlappingItem = nullptr;
  ActiveWeapon = nullptr;
  WeaponToActivate = nullptr;
}

// 플레이어 초기화
void UInventoryComponent::InitializeComponent() {
  Super::InitializeComponent();
  Shooter = Cast<AShooter>(GetOwner());
}

void UInventoryComponent::SetupInputComponent(
    class UInputComponent *InputComponent) {
  // 무기 교체 (1 ~ 5)
	InputComponent->BindAction<FEWeaponTypeDelegate>(
			TEXT("PrimaryGun"), IE_Pressed, this,
			&UInventoryComponent::UseWeaponOfType, EWeaponType::EWT_PrimaryGun);
	InputComponent->BindAction<FEWeaponTypeDelegate>(
			TEXT("SecondaryGun"), IE_Pressed, this,
			&UInventoryComponent::UseWeaponOfType, EWeaponType::EWT_SecondaryGun);
	InputComponent->BindAction<FEWeaponTypeDelegate>(
			TEXT("Pistol"), IE_Pressed, this,
			&UInventoryComponent::UseWeaponOfType, EWeaponType::EWT_Pistol);
	InputComponent->BindAction<FEWeaponTypeDelegate>(
			TEXT("Knife"), IE_Pressed, this,
			&UInventoryComponent::UseWeaponOfType, EWeaponType::EWT_Knife);
	InputComponent->BindAction<FEWeaponTypeDelegate>(
			TEXT("Launcher"), IE_Pressed, this,
			&UInventoryComponent::UseWeaponOfType, EWeaponType::EWT_Launcher);

  // 장착 (E), 무기 사용|집어넣기 (X), 연사 (B), 장전 (R)
	InputComponent->BindAction(
			TEXT("Equip"), IE_Pressed, this,
			&UInventoryComponent::EquipOverlappingWeapon);
	InputComponent->BindAction(
			TEXT("ToggleWeapon"), IE_Pressed, this,
			&UInventoryComponent::ToggleWeapon);
	InputComponent->BindAction(
			TEXT("ToggleAttackMode"), IE_Pressed, this,
			&UInventoryComponent::TryToggleAuto);
	InputComponent->BindAction(
      TEXT("R"), IE_Pressed, this, &UInventoryComponent::Reload);
}

void UInventoryComponent::UseWeaponOfType(EWeaponType Type) {
  if (Weapons[Type]) UseWeapon(Weapons[Type]);
}

void UInventoryComponent::EquipOverlappingWeapon() {
  if (!OverlappingItem ||
      !OverlappingItem->IsOverlappingActor(Shooter)) {
    return;
  }

  // (*) 오버랩 하고 있는 무기 아이템 장착
  const auto &[Type, _] = ExtractTypeAndName(OverlappingItem->GetWeaponBP());
  bool bShouldUse = Weapons[Type] == ActiveWeapon;
  EquipWeapon(OverlappingItem);  // (*)
  if (bShouldUse) UseWeaponOfType(Type);
}

void UInventoryComponent::ToggleWeapon() {
  if (ActiveWeapon) {
    WeaponToActivate = ActiveWeapon;
    UseWeapon(nullptr);
  } else {
    UseWeapon(WeaponToActivate);
  }
}

// 무기 종류에 따라 전환이 불가능할 수도 있음
void UInventoryComponent::TryToggleAuto() {
  if (ActiveWeapon) ActiveWeapon->TryToggleAuto();
}

// 화기류만 장전 가능하며 나이프는 장전 불가능
void UInventoryComponent::Reload() {
	if (AFirearm *Firearm = Cast<AFirearm>(ActiveWeapon)) {
		Firearm->LoadAmmos();
	}
}

// (1) 같은 타입의 무기를 이미 보유 중이면 교체 방법을 설명하는 메시지 출력
// (2) 같은 타입의 무기를 보유 중이지 않다면 즉시 무기 장착
void UInventoryComponent::TryEquipWeapon(class AItem *Item) {
  const auto &[Type, Name] = ExtractTypeAndName(Item->GetWeaponBP());
  if (AWeapon *OldWeapon = Weapons[Type]) {
    LOG("%s(을)를 %s(으)로 교체하려면 E키를 누르세요.",
        *OldWeapon->GetWeaponName(), *Name);  // (1)
    OverlappingItem = Item;
  } else {
    EquipWeapon(Item);  // (2)
  }
}

// (*) 획득한 총알이 사용 중인 무기 타입이면 HUD 업데이트
void UInventoryComponent::EquipAmmo(
    TSubclassOf<class AFirearm> TargetWeapon, uint32 AmmoCount) {
  if (!UnloadedAmmos.Contains(TargetWeapon)) {
    UnloadedAmmos.Add(TargetWeapon, 0);
  }
  UnloadedAmmos[TargetWeapon] += AmmoCount;
  UpdateAmmosIfUsing(TargetWeapon);  // (*)
}

uint32 UInventoryComponent::DecreaseAmmos(
    TSubclassOf<class AFirearm> TargetWeapon, uint32 MaxDecrease) {
  if (!UnloadedAmmos.Contains(TargetWeapon)) return 0;

  // 총알 수를 최대 MaxDecrease만큼 줄이되, 보유 중인 총알이 부족하면 있는 만큼만 줄임
  const uint32 DecreaseAmount =
      FMath::Min(UnloadedAmmos[TargetWeapon], MaxDecrease);
  UnloadedAmmos[TargetWeapon] -= DecreaseAmount;
  UpdateAmmosIfUsing(TargetWeapon);
  return DecreaseAmount;
}

void UInventoryComponent::Save(struct FInventoryStats &InventoryStats) {
  // 무기 리스트 및 총알 정보 저장
  for (const auto &[Type, Weapon] : Weapons) {
    if (!Weapon) continue;
    InventoryStats.Weapons.Add(Type, Weapon->GetClass());
    if (AFirearm *Firearm = Cast<AFirearm>(Weapon)) {
      TSubclassOf<AFirearm> FirearmClass = Firearm->GetClass();
      uint32 AmmoCount = UnloadedAmmos.Contains(FirearmClass) ?
          UnloadedAmmos[FirearmClass] : 0;
      InventoryStats.UnloadedAmmos.Add(FirearmClass, AmmoCount);
      InventoryStats.LoadedAmmos.Add(FirearmClass, Firearm->LoadedAmmos);
    }
  }

  // 마지막으로 사용한 무기 정보 저장
  InventoryStats.ActiveWeaponType =
      ActiveWeapon ? ActiveWeapon->GetType() : EWeaponType::EWT_NoWeapon;
}

void UInventoryComponent::Load(const struct FInventoryStats &InventoryStats) {
  // 무기 리스트 및 총알 정보 로드
  for (const auto &[Type, WeaponClass] : InventoryStats.Weapons) {
    AWeapon *Weapon = EquipWeapon(WeaponClass);
    if (AFirearm *Firearm = Cast<AFirearm>(Weapon)) {
      TSubclassOf<AFirearm> FirearmClass = Firearm->GetClass();
      UnloadedAmmos.Add(
          FirearmClass, InventoryStats.UnloadedAmmos[FirearmClass]);
      Firearm->LoadedAmmos = InventoryStats.LoadedAmmos[FirearmClass];
    }
  }

  // 마지막으로 사용했던 무기 사용
  EWeaponType ActiveWeaponType = InventoryStats.ActiveWeaponType;
  if (ActiveWeaponType != EWeaponType::EWT_NoWeapon) {
    UseWeaponOfType(ActiveWeaponType);
  }
}

// 해당 종류의 총알이 리스트에 없으면 0 반환
uint32 UInventoryComponent::GetUnloadedAmmos(
    TSubclassOf<class AFirearm> TargetWeapon) const {
  return UnloadedAmmos.Contains(TargetWeapon) ?
      UnloadedAmmos[TargetWeapon] : 0; 
}

// 임시로 액터를 생성하여 타입과 이름만 추출 후 즉시 제거
TPair<EWeaponType, FString> UInventoryComponent::ExtractTypeAndName(
    TSubclassOf<class AWeapon> WeaponClass) const {
  AWeapon *Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
  const EWeaponType Type = Weapon->GetType();
  const FString Name = Weapon->GetWeaponName();
  Weapon->Destroy();
  return {Type, Name};
}

void UInventoryComponent::UseWeapon(class AWeapon *NewWeapon) {
  if (NewWeapon == ActiveWeapon) return;

  // 무기 사용 또는 사용 해제 사운드 재생
  USoundBase *PlaySound = NewWeapon ?
      NewWeapon->GetRaiseSound() : ActiveWeapon->GetLowerSound();
  UGameplayStatics::PlaySound2D(this, PlaySound);

  // 화기류 무기에 대해 오프셋을 둠
  Shooter->OffsetPlayer(NewWeapon &&
      NewWeapon->GetType() != EWeaponType::EWT_Knife);

  // 소켓 간 무기 이동
  AttachWeaponToHand(ActiveWeapon, false);
  AttachWeaponToHand(NewWeapon);
  ActiveWeapon = NewWeapon;
  if (HUD) HUD->UpdateWeapon();
}

class AWeapon *UInventoryComponent::EquipWeapon(
    TSubclassOf<class AWeapon> WeaponClass) {
  AWeapon *Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
  Weapon->SetOwner(Shooter);
  AttachWeaponToHand(Weapon, false);  // 등에 무기 부착
  LOG("%s(을)를 장착했습니다.", *Weapon->GetWeaponName());

  // 기존에 사용하던 무기가 (있다면) 제거
  EWeaponType Type = Weapon->GetType();
  if (Weapons[Type]) Weapons[Type]->Destroy();
  Weapons[Type] = Weapon;
  return Weapon;
}

// 아이템 오버랩 사운드 및 이펙트 재생, 아이템은 제거
class AWeapon *UInventoryComponent::EquipWeapon(class AItem *Item) {
  AWeapon *Weapon = EquipWeapon(Item->GetWeaponBP());
  Item->PlayOverlapEffects();
  Item->Destroy();
  return Weapon;
}

void UInventoryComponent::UpdateAmmosIfUsing(
    TSubclassOf<class AFirearm> WeaponBP) {
  if (ActiveWeapon && ActiveWeapon->IsA(WeaponBP)) {
    if (HUD) HUD->UpdateUnloadedAmmos(UnloadedAmmos[WeaponBP]);
  }
}

// 소켓을 사용하지 않는 무기 종류에 대해서는 빈 문자열 반환
FString UInventoryComponent::GetSocketNameFromType(EWeaponType Type) {
  switch (Type) {
    case EWeaponType::EWT_PrimaryGun:   return TEXT("PrimaryGunSocket");
    case EWeaponType::EWT_SecondaryGun: return TEXT("SecondaryGunSocket");
    case EWeaponType::EWT_Launcher:     return TEXT("LauncherSocket");
  }
  return TEXT("");
}

// 무기로 nullptr 전달 가능
void UInventoryComponent::AttachWeaponToHand(
    class AWeapon *Weapon, bool bAttach) {
  if (!Weapon) return;
  Weapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
  FName SocketName = FName(bAttach ?
      TEXT("RightHandSocket") : *GetSocketNameFromType(Weapon->GetType()));
  Weapon->AttachToComponent(Shooter->GetMesh(), 
      FAttachmentTransformRules::KeepRelativeTransform, SocketName);
}