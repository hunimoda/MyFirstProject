#include "Knife.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"

#include "../../Characters/Enemy/Enemy.h"

AKnife::AKnife() {
	Mesh->SetSkeletalMesh(
			ConstructorHelpers::FObjectFinder<USkeletalMesh>(
					TEXT("SkeletalMesh'/Game/MilitaryWeapSilver/Weapons/Knife_A.Knife_A'")).Object);
	Mesh->SetRelativeLocation(FVector(0.35f, -0.51f, -2.01f));
	Mesh->SetRelativeRotation(FRotator(0.f, -109.31f, -14.44f));

	// 콜리전 컴포넌트 생성
	Blade = CreateDefaultSubobject<UBoxComponent>(TEXT("Blade"));
	Blade->SetupAttachment(Mesh);
	Blade->SetRelativeLocation(FVector(0.f, 7.33f, 16.90f));
	Blade->SetRelativeRotation(FRotator(0.f, 0.f, 26.88f));
	Blade->SetRelativeScale3D(FVector(0.04f, 0.0932f, 0.3023f));
	Blade->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));
	DisableBladeCollision();

	// 멤버 변수 초기화
	Type = EWeaponType::EWT_Knife;
	WeaponName = TEXT("나이프");
	Damage = 15.f;
	AttackWaitTime = 0.5f;
	bAutoAttack = true;
	HitFX = ConstructorHelpers::FObjectFinder<UParticleSystem>(
			TEXT("ParticleSystem'/Game/MilitaryWeapSilver/FX/P_Impact_Stone_Small_01.P_Impact_Stone_Small_01'")).Object;
	AttackSound = ConstructorHelpers::FObjectFinder<USoundBase>(
			TEXT("SoundCue'/Game/MilitaryWeapSilver/Sound/Knife/Cues/KnifeA_Swing_Cue.KnifeA_Swing_Cue'")).Object;
	RaiseSound = ConstructorHelpers::FObjectFinder<USoundBase>(
			TEXT("SoundCue'/Game/MilitaryWeapSilver/Sound/Knife/Cues/Knife_Raise_Cue.Knife_Raise_Cue'")).Object;
	LowerSound = ConstructorHelpers::FObjectFinder<USoundBase>(
			TEXT("SoundCue'/Game/MilitaryWeapSilver/Sound/Knife/Cues/Knife_Lower_Cue.Knife_Lower_Cue'")).Object;
}

void AKnife::BeginPlay() {
	Super::BeginPlay();
	Blade->OnComponentBeginOverlap.
			AddDynamic(this, &AKnife::OnBladeBeginOverlap);
}

// @0.23[s]: 오버랩 활성화, @0.84[s]: 오버랩 비활성화
void AKnife::Attack() {
	Super::Attack();
	GetWorldTimerManager().SetTimer(
			EnableTimer, this, &AKnife::EnableBladeCollision, 0.23f);
	GetWorldTimerManager().SetTimer(
			DisableTimer, this, &AKnife::DisableBladeCollision, 0.84f);
}

void AKnife::EnableBladeCollision() {
	Blade->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AKnife::DisableBladeCollision() {
	Blade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 칼날이 에너미와 오버랩한 경우에만 데미지 가함
void AKnife::OnBladeBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult &SweepResult) {
	if (Cast<AEnemy>(OtherActor)) ApplyDamage(OtherActor);
}