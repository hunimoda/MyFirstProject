#include "BaseAnim.h"

// 멤버 변수 초기화
UBaseAnim::UBaseAnim() {
  AttackMontage = nullptr;
  DeathMontage = nullptr;
}

float UBaseAnim::PlayAttackMontage() {
  return PlayRandomSection(AttackMontage);
}

float UBaseAnim::PlayDeathMontage() { return PlayRandomSection(DeathMontage); }

float UBaseAnim::PlayRandomSection(class UAnimMontage *AnimMontage) {
  if (!AnimMontage) return 0.f;

  // 랜덤으로 선택된 섹션을 재생
  int32 RandIndex = GetRandomIndex(AnimMontage);
  const auto &RandomSection = AnimMontage->CompositeSections[RandIndex];
  Montage_Play(AnimMontage);
  Montage_JumpToSection(RandomSection.SectionName, AnimMontage);

  return AnimMontage->GetSectionLength(RandIndex);  // 선택된 섹션의 길이 반환
}

// (1) 섹션들 중에서 "Random_" 접두사가 포함된 섹션의 인덱스만 배열에 추가하고
// (2) 추가된 인덱스들 가운데 랜덤 선택된 인덱스를 반환
int32 UBaseAnim::GetRandomIndex(class UAnimMontage *AnimMontage) const {
  const auto &Sections = AnimMontage->CompositeSections;
  TArray<int32> Indices;
  for (int32 i = 0; i < Sections.Num(); i++) {
    const FString &SectionName = Sections[i].SectionName.ToString();
    if (SectionName.StartsWith(TEXT("Random_"))) {
      Indices.Add(i);  // (1)
    }
  }
  return Indices[FMath::RandRange(0, Indices.Num() - 1)];  // (2)
}