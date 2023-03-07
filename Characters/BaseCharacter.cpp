#include "BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Components/Health/HealthComponent.h"
#include "../Animations/BaseAnim.h"

ABaseCharacter::ABaseCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	// 주먹 콜리전 컴포넌트 생성
	LeftFist = CreateDefaultSubobject<USphereComponent>(TEXT("LeftFist"));
	LeftFist->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	RightFist = CreateDefaultSubobject<USphereComponent>(TEXT("RightFist"));
	RightFist->SetupAttachment(GetMesh(), FName("RightHandSocket"));

	// 체력 컴포넌트 생성
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// 마우스 이동 시 캐릭터 회전 부드럽게 전환
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	// 스켈레탈 메시 콜리전 제거
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// 멤버 변수 초기화
	bUseRandomInterval = false;
	MinAttackInterval = 0.f;
	MaxAttackInterval = 0.f;
	DamagedFX = nullptr;
	HitDamage = 0.f;
	bAttacking = false;
	bAutoAttack = false;
	bDead = false;
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();
	BaseAnim = Cast<UBaseAnim>(GetMesh()->GetAnimInstance());

	// 주먹 컴포넌트는 오직 폰과 오버랩(오버랩 시 OnFistOverlap 호출)
	for (USphereComponent *Fist : {LeftFist, RightFist}) {
		Fist->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));
		Fist->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Fist->OnComponentBeginOverlap.
				AddDynamic(this, &ABaseCharacter::OnFistOverlap);
	}
}

// 감소 후 체력이 0이면 Die, 체력이 0이 아니면 OnTakeDamage 호출
float ABaseCharacter::TakeDamage(
		float Damage, struct FDamageEvent const &DamageEvent,
		AController *EventInstigator, AActor *DamageCauser) {
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (Health->IsZero()) return 0.f;

	Health->Decrease(Damage);
	if (Health->IsZero()) {
		Die(EventInstigator, DamageCauser);
	} else {
		OnTakeDamage(EventInstigator, DamageCauser, Damage);
	}
	return Damage;
}

void ABaseCharacter::RecoverHealth(float Amount) { Health->Increase(Amount); }

void ABaseCharacter::SetFistCollisionEnabled(bool bLeft, bool bEnabled) {
	ECollisionEnabled::Type CollisionType =
			bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;
	(bLeft ? LeftFist : RightFist)->SetCollisionEnabled(CollisionType);
}

// 콜리전 비활성화 및 컨트롤러로부터 제거
// 타이머를 클리어 하여 죽은 후에도 공격 몽타주가 재생되는 문제를 방지
void ABaseCharacter::Die(AController *EventInstigator, AActor *Causer) {
	bDead = true;
	BaseAnim->PlayDeathMontage();

	GetWorldTimerManager().ClearTimer(AttackTimer);
	GetWorldTimerManager().ClearTimer(EndAttackTimer);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetachFromControllerPendingDestroy();
}

void ABaseCharacter::OnTakeDamage(
		AController *EventInstigator, AActor *Causer, float Damage) {}

void ABaseCharacter::TryAttack() { if (!bAttacking) Attack(); }

void ABaseCharacter::Attack() {
	bAttacking = true	;
	const float AttackTime = OnStartAttack();
	if (AttackTime > 0.f) {
		// 일정 시간 경과 후 공격 종료
		GetWorldTimerManager().ClearTimer(EndAttackTimer);
		GetWorldTimerManager().SetTimer(
				EndAttackTimer, this, &ABaseCharacter::EndAttack, AttackTime);
	} else {
		// 즉시 공격 종료. EndAttack 호출 시 무한루프 생성 위험
		bAttacking = false;
	}
}

float ABaseCharacter::OnStartAttack() { return BaseAnim->PlayAttackMontage(); }

void ABaseCharacter::EndAttack() {
	bAttacking = false;
	if (!bAutoAttack) return;

	if (bUseRandomInterval) {
		// 랜덤한 시간 기다렸다가 다시 공격
		float RandomInterval =
				FMath::RandRange(MinAttackInterval, MaxAttackInterval);
		GetWorldTimerManager().ClearTimer(AttackTimer);
		GetWorldTimerManager().SetTimer(
				AttackTimer, this, &ABaseCharacter::Attack, RandomInterval);
	} else {
		Attack();
	}
}

// 이펙트 재생 및 데미지 가함
void ABaseCharacter::OnFistOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult) {
	UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), DamagedFX, OverlappedComponent->GetComponentTransform());
	UGameplayStatics::ApplyDamage(
			OtherActor, HitDamage, GetController(), this, UDamageType::StaticClass());
}
