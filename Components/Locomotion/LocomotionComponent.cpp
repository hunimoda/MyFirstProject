#include "LocomotionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Health/HealthComponent.h"
#include "../../Characters/Shooter/Shooter.h"

// 멤버 변수 초기화
ULocomotionComponent::ULocomotionComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	WalkSpeed = 240.f;
	RunSpeed = 550.f;
	MinRunStamina = 25.f;
	RunDecreaseRate = 4.f;
	MinPitch = -80.f;
	MaxPitch = +80.f;
}

void ULocomotionComponent::BeginPlay() {
	Super::BeginPlay();

	// 멤버 변수 초기화
	Shooter = Cast<AShooter>(GetOwner());
	Controller = Shooter->GetController();
	Stamina = Shooter->GetStaminaComponent();
	Movement = Shooter->GetCharacterMovement();

	Walk();
}

void ULocomotionComponent::TickComponent(
		float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction *ThisTickFunction) {
	if (IsRunning()) {
		Stamina->Decrease(RunDecreaseRate * DeltaTime);
	}
	if (Stamina->GetValue() < MinRunStamina) {
		Walk();
	}
}

void ULocomotionComponent::SetupInputComponent(
		class UInputComponent *InputComponent) {
	UE_LOG(LogTemp, Error, TEXT("SetupInputComponent"));
	// 점프 (spacebar)
	InputComponent->BindAction(
			TEXT("Jump"), IE_Pressed, this, &ULocomotionComponent::Jump);

	// 캐릭터 회전 없이 좌우 보기 (alt)
	InputComponent->BindAction<FBoolDelegate>(
			TEXT("LookAround"), IE_Pressed, this,
			&ULocomotionComponent::SetLookAround, true);
	InputComponent->BindAction<FBoolDelegate>(
			TEXT("LookAround"), IE_Released, this,
			&ULocomotionComponent::SetLookAround, false);

	// 걷기|달리기 전환 (shift)
	InputComponent->BindAction(
			TEXT("Run"), IE_Pressed, this, &ULocomotionComponent::TryRun);
	InputComponent->BindAction(
			TEXT("Run"), IE_Released, this, &ULocomotionComponent::Walk);

	// 전후좌우 움직임 (WASD)	
	InputComponent->BindAxis(
			TEXT("MoveForward"), this, &ULocomotionComponent::MoveForward);
	InputComponent->BindAxis(
			TEXT("MoveRight"), this, &ULocomotionComponent::MoveRight);

	// 상하좌우 보기 (마우스)
	InputComponent->BindAxis(TEXT("LookUp"), this, &ULocomotionComponent::LookUp);
	InputComponent->BindAxis(
			TEXT("LookRight"), this, &ULocomotionComponent::LookRight);
}

void ULocomotionComponent::Jump() { Shooter->Jump(); }

void ULocomotionComponent::SetLookAround(bool bLookAround) {
	Movement->bUseControllerDesiredRotation = !bLookAround;
}

// 달리는 동안 스태미나 양을 확인해야 하므로 틱 함수 활성화
void ULocomotionComponent::TryRun() {
	if (Stamina->GetValue() >= MinRunStamina) {
		Movement->MaxWalkSpeed = RunSpeed;
		SetComponentTickEnabled(true);
	}
}

// 더 이상 스태미나 양을 확인할 필요가 없으므로 틱 함수 비활성화
void ULocomotionComponent::Walk() {
	Movement->MaxWalkSpeed = WalkSpeed;
	SetComponentTickEnabled(false);
}

void ULocomotionComponent::MoveForward(float Scale) {
	FVector Forward = Shooter->GetActorForwardVector();
	Shooter->AddMovementInput(Forward, Scale);
}

void ULocomotionComponent::MoveRight(float Scale) {
	FVector Right = Shooter->GetActorRightVector();
	Shooter->AddMovementInput(Right, Scale);
}

void ULocomotionComponent::LookUp(float Scale) {
	// 변경 후 피치 예상
	FRotator ControlRotation = Controller->GetControlRotation();
	float DesiredPitch = ControlRotation.Pitch + Scale;

	// 실제 피치가 최소|최대값 범위 내로 있도록 클램프
	ControlRotation.Pitch = FMath::ClampAngle(DesiredPitch, MinPitch, MaxPitch);
	Controller->SetControlRotation(ControlRotation);
}

void ULocomotionComponent::LookRight(float Scale) {
	Shooter->AddControllerYawInput(Scale);
}

bool ULocomotionComponent::IsRunning() const {
	float Speed = FVector2D(Shooter->GetVelocity()).Size();
	float ThresholdSpeed = (WalkSpeed + RunSpeed) / 2;
	return Speed > ThresholdSpeed;
}