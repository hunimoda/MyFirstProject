#include "Explosive.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive() {
	// 루트 콜리전 컴포넌트 생성
	TriggerCapsule =
			CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	SetRootComponent(TriggerCapsule);
	TriggerCapsule->SetSimulatePhysics(true);
	TriggerCapsule->SetCollisionProfileName(TEXT("BlockAll"));
	TriggerCapsule->SetNotifyRigidBodyCollision(true);

	// 스태틱 메시 생성
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(TriggerCapsule);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 이펙트 생성
	AttachedFX =
			CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AttachedFX"));
	AttachedFX->SetupAttachment(StaticMesh);

	// 데미지 계산용 콜리전 생성
	KillingRange = CreateDefaultSubobject<USphereComponent>(TEXT("KillingRange"));
	KillingRange->SetupAttachment(TriggerCapsule);

	// 멤버 변수 초기화
	ExplodeFX = nullptr;
	ExplodeFXScale = 1.f;
	AttachedSound = nullptr;
	ExplodeSound = nullptr;
	MaxDamage = 100.f;
	CuspLocation = 0.5f;
	DestroyDelay = 3.f;
	bAutoDestruct = false;
	AutoDestructTime = 0.f;
	Instigator = nullptr;
	bExploded = false;
}

void AExplosive::BeginPlay() {
	Super::BeginPlay();
	TriggerCapsule->OnComponentHit.AddDynamic(this, &AExplosive::Explode);
	UGameplayStatics::SpawnSoundAttached(AttachedSound, GetRootComponent());
	
	// 자동 폭파가 체크된 경우 타이머 동작
	if (bAutoDestruct) {
		GetWorldTimerManager().SetTimer(
				AutoDestructTimer, this, &AExplosive::Explode, AutoDestructTime);
	}
}

// 오버로드 함수들의 베이스 함수
// (*) 힛 이벤트와 타이머에 의해 중복으로 폭파하지 않도록 방지
void AExplosive::Explode(const FVector &FXDirection) {
	if (!bExploded) {
		bExploded = true;  // (*)
		SpawnFXAndSound(FXDirection);
		ApplyDamageToOverlappingActors();
		DestroyAfterDelay();
	}
}

void AExplosive::Explode() {
	FVector MoveDirection = GetVelocity();
	if (MoveDirection.Normalize()) {
		Explode(-MoveDirection);
	} else {
		Explode(FVector::UnitZ());
	}
}

void AExplosive::Explode(
		UPrimitiveComponent *HitComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, FVector NormalImpulse,
		const FHitResult &Hit) {
	Explode(Hit.ImpactNormal);
}

// (1) 폭발 이펙트 및 (2) 폭발 사운드
void AExplosive::SpawnFXAndSound(const FVector &FXDirection) {
	FVector Location = GetActorLocation();
	FRotator FXRotation = FRotationMatrix::MakeFromZ(FXDirection).Rotator();
	FVector FXScale = FVector(ExplodeFXScale);
	UGameplayStatics::SpawnEmitterAtLocation(
			this, ExplodeFX, Location, FXRotation, FXScale);  // (1)
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, Location);  // (2)
}

void AExplosive::ApplyDamageToOverlappingActors() {
	TArray<AActor *> Victims;
	KillingRange->GetOverlappingActors(Victims, APawn::StaticClass());
	for (AActor *Victim : Victims) {
		ApplyDamage(Victim);
	}
}

void AExplosive::ApplyDamage(class AActor *Actor) {
	// 데미지 계산을 위한 파라미터 계산
	float PawnDistance = GetDistanceTo(Actor);
	float NoDamageRadius = KillingRange->GetScaledSphereRadius();
	float DecreaseProfile =
			MaxDamage / (1 - CuspLocation) * (1 - PawnDistance / NoDamageRadius);

	// 실제 데미지 산출 후 폰에게 데미지 가함
	float Damage = FMath::Clamp(DecreaseProfile, 0.f, MaxDamage);
	auto DamageClass = UDamageType::StaticClass();
	UGameplayStatics::ApplyDamage(Actor, Damage, Instigator, this, DamageClass);
}

void AExplosive::DestroyAfterDelay() {
	// 시뮬레이트 & 콜리전 중지
	TriggerCapsule->SetSimulatePhysics(false);
	TriggerCapsule->SetCollisionProfileName(TEXT("NoCollision"));
	TriggerCapsule->SetNotifyRigidBodyCollision(false);

	// 메시 숨김
	StaticMesh->SetHiddenInGame(true);

	// 지연 시간 후 액터 제거
	GetWorldTimerManager().SetTimer(
			DestroyTimer, this, &AExplosive::Destroy, DestroyDelay);
}

void AExplosive::Destroy() { Super::Destroy(); }