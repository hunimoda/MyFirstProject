#include "HealthComponent.h"

UHealthComponent::UHealthComponent() { Value = MaxValue = 100.f; }

// 체력이 증가 후에도 0 ~ 최댓값 범위를 유지하도록 함
void UHealthComponent::Increase(float Amount) {
	SetValue(FMath::Clamp<float>(Value + Amount, 0.f, MaxValue));
}

void UHealthComponent::Decrease(float Amount) { Increase(-Amount); }

bool UHealthComponent::IsZero() const { return FMath::IsNearlyZero(Value); }

void UHealthComponent::SetValue(float NewValue) {
	Value = NewValue;
	OnChangeValue.ExecuteIfBound(GetRatio());
}