#include "Ammo.h"

// 멤버 변수 초기화
AAmmo::AAmmo() {
  MaxEjectAngle = 0.f;
  EjectImpulse = 0.f;
}

void AAmmo::BeginPlay() {
  Super::BeginPlay();

  // 배출된 탄피가 날아가 땅에 구를 수 있게 함
	SetMobility(EComponentMobility::Movable);
  GetStaticMeshComponent()->SetSimulatePhysics(true);
  GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetStaticMeshComponent()->AddImpulse(GetRandomImpulse());

  // 액터 생성 3초 후 제거
  GetWorldTimerManager().SetTimer(DestroyTimer, this, &AAmmo::Destroy, 3.f);
}

FVector AAmmo::GetRandomImpulse() const {
	// 우향 벡터와 수직으로 랜덤한 방향 계산
	float PlanarAngle = FMath::RandRange(0.f, 2 * PI);
	FVector VerticalVector =
			FMath::Cos(PlanarAngle) * GetActorForwardVector() +
			FMath::Sin(PlanarAngle) * GetActorUpVector();
	
	// 탄피 배출 방향 계산
	float EjectAngle = FMath::DegreesToRadians(
      FMath::RandRange(0.f, MaxEjectAngle));
	FVector EjectDirection =
			FMath::Cos(EjectAngle) * GetActorRightVector() +
			FMath::Sin(EjectAngle) * VerticalVector;

	// 충격량 세기 랜덤하게 조정해 반환
	float Strength = FMath::RandRange(0.8f, 1.2f) * EjectImpulse;
	return Strength * EjectDirection;
}

void AAmmo::Destroy() { Super::Destroy(); }