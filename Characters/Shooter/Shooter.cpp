#include "Shooter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../../Controller/ShooterController.h"
#include "../../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../../Components/Health/HealthComponent.h"
#include "../../Components/Inventory/InventoryComponent.h"
#include "../../Components/Locomotion/LocomotionComponent.h"
#include "../../Weapon/Weapon.h"
#include "../../GameMode/MyGameMode.h"
#include "../../SaveGame/MySaveGame.h"

AShooter::AShooter() {
	GetCapsuleComponent()->InitCapsuleSize(32.f, 97.f);

	// 메시 설정
	GetMesh()->SetSkeletalMesh(
			ConstructorHelpers::FObjectFinder<USkeletalMesh>(
					TEXT("SkeletalMesh'/Game/MyFirstProject/Assets/Character/Ch35_nonPBR.Ch35_nonPBR'")).Object);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -97.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(
			ConstructorHelpers::FClassFinder<UAnimInstance>(
					TEXT("AnimBlueprint'/Game/MyFirstProject/Character/ABP_Shooter.ABP_Shooter_C'")).Class);

	// 스프링암 컴포넌트 생성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 57.f));
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bUsePawnControlRotation = true;

	// 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	// 스태미나|인벤토리|이동 컴포넌트 생성
	Stamina = CreateDefaultSubobject<UHealthComponent>(TEXT("Stamina"));
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Locomotion = CreateDefaultSubobject<ULocomotionComponent>(TEXT("Locomotion"));

	// 멤버 변수 초기화
	NormalDecreaseRate = 1.f;
	OffsetInterpSpeed = 10.f;
	PlayerOffset = 120.f;
	MaxRecoveryRate = 5.f;
	TargetOffset = 0.f;
}

void AShooter::BeginPlay() {
	Super::BeginPlay();
	GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));

	// 체력|스태미나 변화 시 HUD를 업데이트 하도록 바인딩
	Health->OnChangeValue.BindUObject(this, &AShooter::UpdateHealth);
	Stamina->OnChangeValue.BindUObject(this, &AShooter::UpdateStamina);
}

// 스프링암 오프셋 보간이 필요하거나 스태미나가 0이 아닌 경우에만 활성화
void AShooter::Tick (float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	InterpSpringArmOffset(DeltaSeconds);
	Stamina->Decrease(NormalDecreaseRate * DeltaSeconds);
	RecoverHealth(MaxRecoveryRate * Stamina->GetRatio() * DeltaSeconds);

	if (ShouldDisableTick()) SetActorTickEnabled(false);
}

void AShooter::SetupPlayerInputComponent(
		UInputComponent *PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 하위 컴포넌트로 바인딩 위임
	Locomotion->SetupInputComponent(PlayerInputComponent);
	Inventory->SetupInputComponent(PlayerInputComponent);

	// 공격 (LMB)
	PlayerInputComponent->BindAction(
			TEXT("LMB"), IE_Pressed, this, &AShooter::OnLMBPressed);
	PlayerInputComponent->BindAction(
			TEXT("LMB"), IE_Released, this, &AShooter::OnLMBReleased);
}

// (*) 세이브 포인트를 통해 저장된 게임 로드 시에는 트랜스폼을 기억하지만,
// 포탈을 통해 저장된 게임 로드 시에는 이를 기억하지 않음(PlayerStart에서 시작)
void AShooter::Load(const struct FPlayerStats &PlayerStats, bool bPlayerStart) {
	if (!bPlayerStart) {
		SetActorTransform(PlayerStats.Transform); // (*)
	}
	Health->SetValue(PlayerStats.Health);
	Stamina->SetValue(PlayerStats.Stamina);
	Inventory->Load(PlayerStats.InventoryStats);
}

void AShooter::Save(struct FPlayerStats &PlayerStats) {
  PlayerStats.Transform = GetActorTransform();
	PlayerStats.Health = Health->GetValue();
  PlayerStats.Stamina = Stamina->GetValue();
	Inventory->Save(PlayerStats.InventoryStats);
}

// 스태미나가 0이 아니게 되므로 틱 함수 활성화
void AShooter::RecoverStamina(float Amount) {
	SetActorTickEnabled(true);
	Stamina->Increase(Amount);
}

// 스프링암 오프셋을 조정하여 화면 중앙 좌측에 플레이어가 오도록 함
void AShooter::OffsetPlayer(bool bOffset) {
	TargetOffset = bOffset ? PlayerOffset : 0.f;
	SetActorTickEnabled(true);
}

void AShooter::Die(AController *EventInstigator, AActor *Causer) {
	Super::Die(EventInstigator, Causer);
	SetActorTickEnabled(false);

	// 3초 후 게임 모드로 처리 전달
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(
			Timer, GameMode, &AMyGameMode::HandlePlayerDeath, 3.f);
}

float AShooter::OnStartAttack() {
	// 무장 시 해당 무기로 공격 시도
	if (AWeapon *Weapon = Inventory->GetActiveWeapon()) {
		Weapon->TryAttack();
		return 0.f;
	}
	return Super::OnStartAttack();
}

void AShooter::OnLMBPressed() {
	SetAutoAttack(true);
	TryAttack();
}

void AShooter::OnLMBReleased() { SetAutoAttack(false); }

void AShooter::InterpSpringArmOffset(float DeltaSeconds) {
	float CurrentOffset = CameraBoom->SocketOffset.Y;
	float NewOffset = FMath::FInterpTo(
			CurrentOffset, TargetOffset, DeltaSeconds, OffsetInterpSpeed);
	CameraBoom->SocketOffset.Y = NewOffset;
}

// (1) 오프셋 보간 완료, (2) 스태미나가 0인 두 조건을 모두 만족시켜야 틱 함수 중지
bool AShooter::ShouldDisableTick() const {
	float CurrentOffset = CameraBoom->SocketOffset.Y;
	bool bInterpCompleted = FMath::IsNearlyEqual(
			CurrentOffset, TargetOffset, 0.1f);
	return bInterpCompleted && Stamina->IsZero();
}

void AShooter::UpdateHealth(float Ratio) { if (HUD) HUD->UpdateHealth(Ratio); }

void AShooter::UpdateStamina(float Ratio) {
	if (HUD) HUD->UpdateStamina(Ratio);
}