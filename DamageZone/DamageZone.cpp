#include "DamageZone.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "../Components/PeriodicalMovement/PeriodicalMovementComponent.h"

ADamageZone::ADamageZone() {
  PrimaryActorTick.bCanEverTick = true;

  // 콜리전 컴포넌트 생성: 폰 채널과만 오버랩
  TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
  TriggerBox->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));
  SetRootComponent(TriggerBox);

  // 기타 컴포넌트 생성
  IdleFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleFX"));
  IdleFX->SetupAttachment(TriggerBox);
  PeriodicalMovement = CreateDefaultSubobject<
      UPeriodicalMovementComponent>(TEXT("PeriodicalMovement"));

  // 멤버 변수 초기화
  DamageRate = 0.f;
}

void ADamageZone::BeginPlay() {
  Super::BeginPlay();

  // 오버랩 이벤트 델리게이트 등록
  TriggerBox->OnComponentBeginOverlap.
      AddDynamic(this, &ADamageZone::OnBeginOverlap);
  TriggerBox->OnComponentEndOverlap.
      AddDynamic(this, &ADamageZone::OnEndOverlap);

  SetActorTickEnabled(false);
}

void ADamageZone::Tick(float DeltaSeconds) {
  Super::Tick(DeltaSeconds);

  // 오버랩 중인 폰에 데미지 입힘
  const float Damage = DamageRate * DeltaSeconds;
  for (int32 i = 0; i < PawnsToDamage.Num(); i++) {
    UGameplayStatics::ApplyDamage(
        PawnsToDamage[i], Damage, nullptr, this, UDamageType::StaticClass());
  }
}

// 폰과 오버랩 시 데미지를 가할 대상으로 등록하고 틱 활성화
// 실제로 데미지를 가하는 과정은 틱 함수 내에서 이루어짐
void ADamageZone::OnBeginOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult &SweepResult) {
  if (APawn *Pawn = Cast<APawn>(OtherActor)) {
    PawnsToDamage.Add(Pawn);
    SetActorTickEnabled(true);
  }
}

// 폰이 데미지 존을 벗어나면 데미지를 가할 대상에서 제거
// 만일 데미지를 가할 대상이 하나도 없다면 틱을 중지
void ADamageZone::OnEndOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) {
  if (APawn *Pawn = Cast<APawn>(OtherActor)) {
    PawnsToDamage.Remove(Pawn);
    if (!PawnsToDamage.Num()) {
      SetActorTickEnabled(false);
    }
  }
}