#include "ShooterOverlap.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../Components/PeriodicalMovement/PeriodicalMovementComponent.h"
#include "../GameMode/MyGameMode.h"
#include "../Characters/Shooter/Shooter.h"

AShooterOverlap::AShooterOverlap() {
  // 루트 콜리전 컴포넌트 생성: 폰 채널과만 오버랩
  OverlapSphere = CreateDefaultSubobject<
      USphereComponent>(TEXT("OverlapSphere"));
  SetRootComponent(OverlapSphere);
  OverlapSphere->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));

  // 기타 컴포넌트 생성
  StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
  StaticMesh->SetupAttachment(OverlapSphere);
  IdleFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleFX"));
  IdleFX->SetupAttachment(OverlapSphere);
  PeriodicalMovement = CreateDefaultSubobject<
      UPeriodicalMovementComponent>(TEXT("PeriodicalMovement"));

  // 멤버 변수 초기화
  bPlayEffectsOnOverlap = true;
  OverlapSound = nullptr;
  OverlapFX = nullptr;
}

void AShooterOverlap::PlayOverlapEffects() {
  UGameplayStatics::PlaySound2D(this, OverlapSound);
  UGameplayStatics::SpawnEmitterAtLocation(
      GetWorld(), OverlapFX, GetActorTransform());
}

void AShooterOverlap::BeginPlay() {
  Super::BeginPlay();
  GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
  OverlapSphere->OnComponentBeginOverlap.
      AddDynamic(this, &AShooterOverlap::OnSphereBeginOverlap);
}

void AShooterOverlap::OnShooterOverlap(class AShooter *Shooter) {
  if (bPlayEffectsOnOverlap) PlayOverlapEffects();
}

// 플레이어와 오버랩 한 경우에만 OnShooterOverlap 함수를 호출하므로 호출된 함수 안에서 Shooter 변수는 nullptr가 아님이 보장됨
void AShooterOverlap::OnSphereBeginOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, 
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult &SweepResult) {
  if (AShooter *Shooter = Cast<AShooter>(OtherActor)) {
    OnShooterOverlap(Shooter);
  }
}