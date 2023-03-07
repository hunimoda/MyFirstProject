#include "Enemy.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"

#include "../Shooter/Shooter.h"
#include "../../Components/Health/HealthComponent.h"
#include "../../GameMode/MyGameMode.h"
#include "../../HUD/EnemyHealthBar/EnemyHealthBar.h"
#include "../../SaveGame/MySaveGame.h"

AEnemy::AEnemy() {
	// 체력 컴포넌트 생성
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetWidgetClass(
			ConstructorHelpers::FClassFinder<UEnemyHealthBar>(
					TEXT("WidgetBlueprint'/Game/MyFirstProject/HUD/WBP_EnemyHealthBar.WBP_EnemyHealthBar_C'")).Class);
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize(FVector2D(300.f, 10.f));

	// 추격|공격 가능 범위 컴포넌트 생성 후 폰 채널과만 오버랩 하도록 설정
	ChaseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseSphere"));
	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	for (auto Sphere : {ChaseSphere, AttackSphere}) {
		Sphere->SetupAttachment(GetRootComponent());
		Sphere->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));
	}

	// 멤버 변수 초기화|오버라이드
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseRandomInterval = true;
	MinAttackInterval = 3.f;
	MaxAttackInterval = 3.f;
	IdleTime = 0.f;
	SinkSpeed = 0.f;
	BehaviorStatus = EBehaviorStatus::EBS_Spawned;
	PatrolIndex = -1;
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();

	// 추격 범위 오버랩 델리게이트 등록
	ChaseSphere->OnComponentBeginOverlap.
			AddDynamic(this, &AEnemy::OnChaseSphereBeginOverlap);
	ChaseSphere->OnComponentEndOverlap.
			AddDynamic(this, &AEnemy::OnChaseSphereEndOverlap);

	// 게임 모드, AI 컨트롤러, 플레이어 초기화
	GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
	AIController = Cast<AAIController>(GetController());
	PathFollowing = AIController->GetPathFollowingComponent();
	Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this, 0));
	WHealthBar = Cast<UEnemyHealthBar>(HealthBar->GetUserWidgetObject());

	SetActorTickEnabled(false);
	TransitTo(EBehaviorStatus::EBS_Idle);
	ConvertPatrolPointsToWorld();
	DisplayHealthBar(false);
}

void AEnemy::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (BehaviorStatus == EBehaviorStatus::EBS_Sink) {
		Sink(DeltaSeconds);  // 하강
	} else if (AIController->LineOfSightTo(Shooter)) {
		if (AttackSphere->IsOverlappingActor(Shooter)) {
			// 공격 범위 내에서 플레이어가 보이면 공격
			TransitTo(EBehaviorStatus::EBS_Attack);
		} else if (!IsAttacking()) {
			// 공격 범위 밖에서 플레이어가 보이고 현재 공격 중이지 않으면 추격
			TransitTo(EBehaviorStatus::EBS_Chase);
		}
	} else if (BehaviorStatus == EBehaviorStatus::EBS_Attack) {
		// 공격 상태에서 플레이어가 보이지 않게 되면 추격
		TransitTo(EBehaviorStatus::EBS_Chase);
	}

	// 게임 시작 즉시 추격 범위와 오버랩 시 체력 바가 보이지 않는 문제 해결
	WHealthBar->SetRatio(Health->GetRatio());
}

void AEnemy::Load(const struct FEnemyStats &Stats) {
	SetActorTransform(Stats.Transform);
	Health->SetValue(Stats.Health);
	PatrolPoints = Stats.PatrolPoints;
	PatrolIndex = Stats.PatrolIndex;
}

void AEnemy::Save(struct FEnemyStats &Stats) {
	Stats.Transform = GetActorTransform();
	Stats.Health = Health->GetValue();
	Stats.PatrolPoints = PatrolPoints;
	Stats.PatrolIndex = PatrolIndex;
}

void AEnemy::Die(AController *EventInstigator, AActor *Causer) {
	Super::Die(EventInstigator, Causer);
	TransitTo(EBehaviorStatus::EBS_Dead);
	GameMode->HandleEnemyDeath(EventInstigator);
	DisplayHealthBar(false);  // 죽을 때 체력 바를 숨김
}

void AEnemy::OnTakeDamage(AController *EventInstigator,
													AActor *Causer, float Damage) {
	WHealthBar->SetRatio(Health->GetRatio());
	DisplayHealthBarForTime();

	// 데미지를 입힌 것이 플레이어면 추격
	if (EventInstigator == Shooter->GetController()) {
		TransitTo(EBehaviorStatus::EBS_Chase);
	}
}

void AEnemy::TransitTo(EBehaviorStatus NewBehaviorStatus) {
	// 같은 상태로 전이할 수 없음
	if (BehaviorStatus == NewBehaviorStatus) return;
	BehaviorStatus = NewBehaviorStatus;

	// 상태 정보를 초기화하고 다른 상태로 전이
	InitializeStateVariables();
	switch (BehaviorStatus) {
		case EBehaviorStatus::EBS_Idle:		TransitToIdle();		break;
		case EBehaviorStatus::EBS_Patrol:	TransitToPatrol();	break;
		case EBehaviorStatus::EBS_Chase:	TransitToChase();		break;
		case EBehaviorStatus::EBS_Attack:	TransitToAttack();	break;
		case EBehaviorStatus::EBS_Dead:		TransitToDead();		break;
		case EBehaviorStatus::EBS_Sink:		TransitToSink();		break;
	}
}

// OnRequestFinished 델리게이트
void AEnemy::TransitTo(struct FAIRequestID RequestID,
											 const struct FPathFollowingResult &Result,
											 EBehaviorStatus NewBehaviorStatus) {
	TransitTo(NewBehaviorStatus);
}

// 일정 시간 경과 후 순찰 상태로 전이
void AEnemy::TransitToIdle() {
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &AEnemy::TransitTo, EBehaviorStatus::EBS_Patrol);
	GetWorldTimerManager().SetTimer(IdleToPatrolTimer, Delegate, IdleTime, false);
}

// 다음 순찰 지점으로 이동, 도착 시 아이들링으로 전환
void AEnemy::TransitToPatrol() {
	PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
	AIController->MoveToLocation(PatrolPoints[PatrolIndex]);
	PathFollowing->OnRequestFinished.AddUObject(
			this, &AEnemy::TransitTo, EBehaviorStatus::EBS_Idle);
}

void AEnemy::TransitToChase() { AIController->MoveToActor(Shooter); }

void AEnemy::TransitToAttack() {
	SetAutoAttack(true);
	Attack();
}

void AEnemy::TransitToDead() {
	SetActorTickEnabled(false);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &AEnemy::TransitTo, EBehaviorStatus::EBS_Sink);
	GetWorldTimerManager().SetTimer(DeathToSinkTimer, Delegate, 10.f, false);
}

void AEnemy::TransitToSink() {
	SetActorTickEnabled(true);
	GetWorldTimerManager().SetTimer(
			SinkToDestroyTimer, this, &AEnemy::DestroyEnemy, 5.f);
}

void AEnemy::DestroyEnemy() { Destroy(); }

// 시작 위치를 마지막 순찰 지점으로 추가 후 모든 순찰 지점을 월드 좌표로 변환
void AEnemy::ConvertPatrolPointsToWorld() {
	PatrolPoints.Add(FVector::ZeroVector);
	const FTransform &Transform = GetActorTransform();
	for (FVector &PatrolPoint : PatrolPoints) {
		PatrolPoint = UKismetMathLibrary::TransformLocation(Transform, PatrolPoint);
	}
}

// 5초간 체력 바를 보였다가 숨김
void AEnemy::DisplayHealthBarForTime() {
	DisplayHealthBar(true);
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
			HealthBarTimer, this, &AEnemy::OnDisplayHealthBarTimeout, 5.f);
}

void AEnemy::OnDisplayHealthBarTimeout() {
	if (!ChaseSphere->IsOverlappingActor(Shooter)) {
		DisplayHealthBar(false);
	}
}

void AEnemy::DisplayHealthBar(bool bDisplay) {
	WHealthBar->DisplayHealthBar(bDisplay);
}

// 에너미가 살아있는 동안 플레이어가 추격 범위에 진입하면,
// 틱 함수를 활성화 시켜 감시를 진행하고 머리 위에 체력 바를 보임
void AEnemy::OnChaseSphereBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult) {
	if (OtherActor == Shooter && !IsDead()) {
		SetActorTickEnabled(true);
		DisplayHealthBar(true);
	}
}

// (1) 플레이어가 추격 범위를 벗어나면 하강 상태를 제외하고 틱 불필요
// (2) 공격|추격 중이었다면 휴식
void AEnemy::OnChaseSphereEndOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) {
	if (OtherActor == Shooter) {
		if (BehaviorStatus != EBehaviorStatus::EBS_Sink) {
			SetActorTickEnabled(false);  // (1)
		}
		if (BehaviorStatus == EBehaviorStatus::EBS_Chase ||
				BehaviorStatus == EBehaviorStatus::EBS_Attack) {
			TransitTo(EBehaviorStatus::EBS_Idle);  // (2)
		}
		DisplayHealthBar(false);
	}
}

void AEnemy::Sink(float DeltaSeconds) {
	AddActorWorldOffset(-SinkSpeed * FVector::UnitZ() * DeltaSeconds);
}

// 타이머|자동 공격|움직임|델리게이트 등 상태 전이 시 설정된 정보를 모두 초기화
void AEnemy::InitializeStateVariables() {
	GetWorldTimerManager().ClearTimer(IdleToPatrolTimer);
	SetAutoAttack(false);
	if (!IsDead()) {
		AIController->StopMovement();
		PathFollowing->OnRequestFinished.Clear();
	}
}