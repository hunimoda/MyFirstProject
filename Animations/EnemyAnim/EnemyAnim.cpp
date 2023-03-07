#include "EnemyAnim.h"

#include "../../Characters/Enemy/Enemy.h"

// 멤버 변수 초기화
UEnemyAnim::UEnemyAnim() {
  Speed = 0.f;
  BehaviorStatus = EBehaviorStatus::EBS_Spawned;
}

void UEnemyAnim::NativeBeginPlay() {
  Super::NativeBeginPlay();
  Enemy = Cast<AEnemy>(GetOwningActor());
}

// 에너미 속력 및 활동 상태 업데이트
void UEnemyAnim::NativeUpdateAnimation(float DeltaSeconds) {
  Super::NativeUpdateAnimation(DeltaSeconds);
  if (Enemy) {
    Speed = FVector2D(Enemy->GetVelocity()).Size();
    BehaviorStatus = Enemy->GetBehaviorStatus();
  }
}