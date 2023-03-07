#include "HeadsUpDisplay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "../../Characters/Shooter/Shooter.h"
#include "../../Components/Inventory/InventoryComponent.h"
#include "../../Weapon/Firearm/Firearm.h"
#include "../../GameMode/MyGameMode.h"

UHeadsUpDisplay::UHeadsUpDisplay(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer) {
  // 크로스헤어 맵 초기화
  Crosshairs.Add(EWeaponType::EWT_PrimaryGun,    {nullptr, 0});
  Crosshairs.Add(EWeaponType::EWT_SecondaryGun,  {nullptr, 0});
  Crosshairs.Add(EWeaponType::EWT_Pistol,        {nullptr, 0});
  Crosshairs.Add(EWeaponType::EWT_Knife,         {nullptr, 0});
  Crosshairs.Add(EWeaponType::EWT_Launcher,      {nullptr, 0});
  Crosshairs.Add(EWeaponType::EWT_NoWeapon,      {nullptr, 0});

  // 무기 이미지 맵 초기화
  WeaponTextures.Add(EWeaponType::EWT_PrimaryGun,   nullptr);
  WeaponTextures.Add(EWeaponType::EWT_SecondaryGun, nullptr);
  WeaponTextures.Add(EWeaponType::EWT_Pistol,       nullptr);
  WeaponTextures.Add(EWeaponType::EWT_Knife,        nullptr);
  WeaponTextures.Add(EWeaponType::EWT_Launcher,     nullptr);
  WeaponTextures.Add(EWeaponType::EWT_NoWeapon,     nullptr);

  // 멤버 변수 초기화
  LogDisplayTime = 5.f;
  StaminaBarNormalColor = FLinearColor();
  StaminaBarShortageColor = FLinearColor();
  MinimapMinSize = 250.f;
  MinimapMaxSize = 450.f;
}

// 멤버 변수 초기화
void UHeadsUpDisplay::NativeConstruct() {
  Super::NativeConstruct();
  GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
  Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this, 0));
  bLargeMinimap = false;
  Minimap->SetDesiredSizeOverride(FVector2D(MinimapMinSize));
}

void UHeadsUpDisplay::UpdatePlayerLives() {
  PlayerLives->SetText(FText::AsNumber(GameMode->GetPlayerLives()));
}

// (*) 0 ~ 9초(한 자릿수) 범위에서 두 자릿수로 표시 i.e. 00 ~ 09초
void UHeadsUpDisplay::UpdatePlayTime(uint32 Minute, uint32 Second) {
  FString SecondAsStr = FString::FromInt(Second);
  if (SecondAsStr.Len() == 1) {
    SecondAsStr = "0" + SecondAsStr;  // (*)
  }
  MinuteText->SetText(FText::AsNumber(Minute));
  SecondText->SetText(FText::FromString(SecondAsStr));
}

void UHeadsUpDisplay::UpdateEnemyCount() {
  EnemyCount->SetText(FText::AsNumber(GameMode->GetEnemyCount()));
}

// 로그 메시지를 보인 후 일정 시간이 경과하면 숨김
void UHeadsUpDisplay::DisplayLogMessage(const FString &Message) {
  LogMessage->SetText(FText::FromString(Message));
  GetWorld()->GetTimerManager().ClearTimer(HideLogTimer);
  GetWorld()->GetTimerManager().SetTimer(
      HideLogTimer, this, &UHeadsUpDisplay::HideLogMessage,
      LogDisplayTime, false);
}

void UHeadsUpDisplay::UpdateWeapon() {
  UInventoryComponent *Inventory = Shooter->GetInventory();
  AWeapon *Weapon = Inventory->GetActiveWeapon();
  EWeaponType WeaponType = Weapon ?
      Weapon->GetType() : EWeaponType::EWT_NoWeapon;

  DisplayWeaponStats((bool)Weapon);  // 무장했을 때에만 WeaponStats 위젯을 보임

  // 무기 종류에 맞는 이미지와 크로스헤어로 변경
  UpdateCrosshair(WeaponType);
  UpdateWeaponImage(WeaponType);
  if (!Weapon) return;

  // 총알 수 및 연사 모드 업데이트
  if (AFirearm *Firearm = Cast<AFirearm>(Weapon)) {
    UpdateUnloadedAmmos(Shooter->GetInventory()->
        GetUnloadedAmmos(Firearm->GetClass()));
    UpdateLoadedAmmos(Firearm->LoadedAmmos);
    DisplayAuto(Firearm->IsAutoAttack());
  } else {
    // 나이프로 무장 시 총알 수가 '(빈칸)/1'로 표시
    UpdateUnloadedAmmos(1);
    UpdateLoadedAmmos();
    DisplayAuto(false);
  }
}

void UHeadsUpDisplay::DisplayAuto(bool bDisplay) {
  AutoMode->SetVisibility(
      bDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHeadsUpDisplay::UpdateLoadedAmmos() {
  LoadedAmmosText->SetText(FText::FromString(""));
}

void UHeadsUpDisplay::UpdateLoadedAmmos(uint32 LoadedAmmos) {
  LoadedAmmosText->SetText(FText::AsNumber(LoadedAmmos));
}

void UHeadsUpDisplay::UpdateUnloadedAmmos(uint32 UnloadedAmmos) {
  UnloadedAmmosText->SetText(FText::AsNumber(UnloadedAmmos));
}

void UHeadsUpDisplay::UpdateStamina(float Stamina) {
  StaminaBar->SetPercent(Stamina);
}

void UHeadsUpDisplay::UpdateHealth(float Health) {
  HealthBar->SetPercent(Health);
}

void UHeadsUpDisplay::ToggleMinimapSize() {
  bLargeMinimap ^= true;
  float MinimapSize = bLargeMinimap ? MinimapMaxSize : MinimapMinSize;
  Minimap->SetDesiredSizeOverride(FVector2D(MinimapSize));
}

// 로그 메시지를 빈 문자열로 설정해 메시지를 숨김
void UHeadsUpDisplay::HideLogMessage() {
  LogMessage->SetText(FText::FromString(""));
}

void UHeadsUpDisplay::DisplayWeaponStats(bool bDisplay) {
  WeaponStats->SetVisibility(
      bDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHeadsUpDisplay::UpdateCrosshair(EWeaponType Type) {
  Crosshair->SetBrushFromTexture(Crosshairs[Type].Texture);
  Crosshair->SetDesiredSizeOverride(FVector2D(Crosshairs[Type].Size));
}

void UHeadsUpDisplay::UpdateWeaponImage(EWeaponType Type) {
  if (UTexture2D *WeaponTexture = WeaponTextures[Type]) {
    WeaponImage->SetBrushFromTexture(WeaponTexture);
    WeaponImage->SetDesiredSizeOverride(
        FVector2D(WeaponTexture->GetSizeX(), WeaponTexture->GetSizeY()));
  }
}