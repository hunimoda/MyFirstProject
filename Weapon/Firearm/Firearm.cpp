#include "Firearm.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

#include "../../Controller/ShooterController.h"
#include "../../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../../Characters/Shooter/Shooter.h"
#include "../../Components/Inventory/InventoryComponent.h"

AFirearm::AFirearm() {
	// 손에 맞게 총기 위치 조정
	Mesh->SetRelativeLocation(FVector(-0.65f, 0.72f, -2.28f));
	Mesh->SetRelativeRotation(FRotator(0.f, -97.35f, 0.81f));
	
	// 멤버 변수 초기화
	HitFX = ConstructorHelpers::FObjectFinder<UParticleSystem>(
			TEXT("ParticleSystem'/Game/MilitaryWeapSilver/FX/P_Impact_Stone_Large_01.P_Impact_Stone_Large_01'")).Object;
	Type = EWeaponType::EWT_PrimaryGun;
	LoadedAmmos = 0;
	MaxLoadAmmos = 0;
	MuzzleFX = nullptr;
	MuzzleRearFX = nullptr;
	ReloadSound = nullptr;
	NoAmmoSound = ConstructorHelpers::FObjectFinder<USoundBase>(
			TEXT("SoundWave'/Game/MyFirstProject/Assets/Sound/NoAmmo.NoAmmo'")).Object;
}

void AFirearm::LoadAmmos() {
	// 인벤토리에서 총알을 최대한 꺼냄
	UInventoryComponent *Inventory = Shooter->GetInventory();
	uint32 TargetLoad = MaxLoadAmmos - LoadedAmmos;
	uint32 ActualLoad = Inventory->DecreaseAmmos(GetClass(), TargetLoad);
	
	// 장전에 성공한 경우
	if (ActualLoad) {
		LoadedAmmos += ActualLoad;
		if (HUD) HUD->UpdateLoadedAmmos(LoadedAmmos);
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}
}

// 장전된 총알이 없으면 공격 불가
bool AFirearm::CanAttack() { return Super::CanAttack() && LoadedAmmos; }

void AFirearm::Attack() {
	Super::Attack();
	LoadedAmmos--;  // 총알 감소
	if (HUD) HUD->UpdateLoadedAmmos(LoadedAmmos);

	// 발사 이펙트
	UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, FName("MuzzleFlash"));
	UGameplayStatics::SpawnEmitterAttached(
			MuzzleRearFX, Mesh, FName("MuzzleFlashRear"));
}

// 빈 탄창 사운드 재생
void AFirearm::OnAttackFailure() {
	if (!LoadedAmmos) {
		UGameplayStatics::PlaySound2D(this, NoAmmoSound);
	}
}