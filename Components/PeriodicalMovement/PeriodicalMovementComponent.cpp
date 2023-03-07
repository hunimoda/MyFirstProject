#include "PeriodicalMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// 멤버 변수 초기화
UPeriodicalMovementComponent::UPeriodicalMovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	bShouldTranslate = false;
	DeltaPoint = FVector(0.f);
	TravelTime = 0.f;
	WaitTime = 0.f;
	bShouldRotate = false;
	RotatePeriod = 0.f;
}

// 왕복 운동 시 시작점과 반환점 초기화
void UPeriodicalMovementComponent::BeginPlay() {
	Super::BeginPlay();
	SetComponentTickEnabled(bShouldTranslate || bShouldRotate);
	InitialPoint = GetOwner()->GetActorLocation();
	TurningPoint = InitialPoint + DeltaPoint;
}

void UPeriodicalMovementComponent::TickComponent(
		float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bShouldTranslate) Translate();  // 왕복 운동
	if (bShouldRotate) Rotate(DeltaTime);  // 회전 운동
}

// 내분점 공식으로 위치 계산
void UPeriodicalMovementComponent::Translate() {
	float Alpha = GetAlpha(GetModuloTime());
	FVector NextLocation = Alpha * TurningPoint + (1 - Alpha) * InitialPoint;
	GetOwner()->SetActorLocation(NextLocation);
}

float UPeriodicalMovementComponent::GetModuloTime() const {
	const double TimeSeconds = GetWorld()->GetTimeSeconds();
	const double TranslatePeriod = 2 * (TravelTime + WaitTime);
	double ModuloTime;
	UKismetMathLibrary::FMod(TimeSeconds, TranslatePeriod, ModuloTime);
	return ModuloTime;
}

// 한 사이클 내의 시간 값으로부터 0에서 1 사이의 값(내분점 계수)을 적절히 반환
// (1) 터닝 포인트로 이동
// (2) 터닝 포인트에 도착
// (3) 처음 위치에 도착
// (4) 처음 위치로 이동
float UPeriodicalMovementComponent::GetAlpha(float Time) const {
	if (Time < TravelTime) return GetAlphaBeforeTravelTime(Time);  // (1)
	if (Time < TravelTime + WaitTime) return 1.f;  // (2)
	if (Time > 2 * TravelTime + WaitTime) return 0.f;  // (3)

	Time -= TravelTime + WaitTime;
	return 1.f - GetAlphaBeforeTravelTime(Time);  // (4)
}

float UPeriodicalMovementComponent::GetAlphaBeforeTravelTime(float Time) const {
	return 0.5f * (1 - FMath::Cos(PI * Time / TravelTime));
}

// 일정한 속도로 각 변위 추가
void UPeriodicalMovementComponent::Rotate(float DeltaTime) {
  float DeltaDegree = (360.f / RotatePeriod) * DeltaTime;
	FRotator DeltaRotator = FRotator(0.f, DeltaDegree, 0.f);
	GetOwner()->AddActorWorldRotation(DeltaRotator);
}