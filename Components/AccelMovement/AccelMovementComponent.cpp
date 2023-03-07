#include "AccelMovementComponent.h"

// 멤버 변수 초기화
UAccelMovementComponent::UAccelMovementComponent() {
  PrimaryComponentTick.bCanEverTick = true;
  Acceleration = 0.f;
  MaxSpeed = 0.f;
}

void UAccelMovementComponent::BeginPlay() {
  Super::BeginPlay();
  UpVector = GetOwner()->GetActorUpVector();

  // 루트 컴포넌트는 Simulate Physics가 설정된 UPrimitiveComponent이어야 함
  RootComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
  if (!RootComponent) {
    UE_LOG(LogTemp, Fatal,
           TEXT("%s의 루트 컴포넌트가 UPrimitiveComponent 타입이 아님."),
           *GetOwner()->GetName());
  } else if (!RootComponent->IsSimulatingPhysics()) {
    UE_LOG(LogTemp, Error, TEXT("%s에 Simulate Physics가 설정되어 있지 않음."),
           *RootComponent->GetName());
  }
}

void UAccelMovementComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // 최대 속도에 못 미치고 시뮬레이트가 활성화 되어 있을 때만 가속
  bool bAtMaxSpeed = GetOwner()->GetVelocity().Size() > MaxSpeed;
  if (bAtMaxSpeed || !RootComponent->IsSimulatingPhysics()) {
    SetComponentTickEnabled(false);
  } else {
    RootComponent->AddForce(
        RootComponent->GetMass() * Acceleration * UpVector);
  }
}