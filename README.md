_<div align="right">Last update: 2023. 4. 2.</div>_

<!-- 제목 ---------------------------------------------->
# MyFirstProject
<br>
<div align="center">
  <img src="https://compote.slate.com/images/697b023b-64a5-49a0-8059-27b963453fb1.gif" width="720" />

  *[플레이 영상(일부) - [전체 영상 링크](https://www.google.com/webhp?hl=en)]*
</div>
<br>

<!-- 개요 ---------------------------------------------->
## 개요
- 프로젝트명: **MyFirstProject**
- 수행 기간: **2023. 1. ~ 2023. 2.**
- 참여 인원: **1인**
- 게임 장르: **3인칭 슈팅 + 사냥**
- 사용 기술: **C++, Unreal Engine v5.0.3**  
<br>

<!-- 목차 ---------------------------------------------->
## 목차
- [게임 소개](#게임-소개)
- 전체 구성
  - [게임 시작](#게임-시작)
- 세부 구현
  - [몬스터의 상태 변화](#몬스터의-상태-변화)
  - [몬스터 순찰 지점 등록](#몬스터-순찰-지점-등록)
  - [스태미나 소모로 체력 회복](#스태미나-소모로-체력-회복)
  - [걷기-달리기 전환](#걷기-달리기-전환)
  - [무기 장착](#무기-장착)
  - [주기운동 컴포넌트](#주기운동-컴포넌트-uperiodicalmovementcomponent)
  - [폭약](#폭약-aexplosive)
  - [포털](#포털-aportal)
  - [오버랩 베이스 클래스](#오버랩-베이스-클래스-ashooteroverlap)
- [추가로 구현하고 싶은 기능](#추가로-구현하고-싶은-기능들)
<br>

<!-- 게임 소개 ------------------------------------------>
## 게임 소개
이 게임은 PC에서 실행되며, 1인용 **TPS**(3인칭 슈팅) 및 **사냥** 게임이다. 맵에는 다수의 몬스터가 존재하며, 플레이어는 이들을 처치하고 레벨을 클리어하여 다음 레벨로 진행한다. 모든 레벨을 클리어하면 플레이어가 승리한다. 플레이어는 몬스터의 공격을 받을 수 있으며, 최대 세 개의 목숨이 부여된다. 모든 목숨을 소진하면 플레이어는 패배한다.

플레이어는 **맨손, 단검, 각종 총기류 및 폭약 등**을 사용하여 몬스터에게 피해를 입힐 수 있다. 그러나, **몬스터나 각종 장애물**(불, 독가스, 부비트랩)에 의해 플레이어가 피해를 입을 수도 있다. 플레이어가 피해를 입으면 체력이 감소하지만, 체력 포션을 사용하여 회복할 수 있다. 또한, 스태미나 상태가 관리되며 일정 수준 이상이면 달리기가 가능하고, 스태미나가 채워진 양에 비례하여 체력이 지속적으로 회복된다. 단, 달리는 동안 체력은 회복되지 않는다.

총기류는 총알이 있어야만 사용할 수 있으며, 총알이 바닥나면 총기는 발사되지 않는다. 무기, 총알 및 포션 등을 아이템으로 획득할 수 있다. 각 레벨에는 여러 개의 **세이브 포인트**가 배치되어 있으며, 이 포인트와 겹치면 게임 진행 상황이 저장된다. 따라서, 다음 플레이 시 이전에 저장한 지점에서 게임을 이어나갈 수 있다. 그러나, 모든 목숨을 소진하면 세이브 포인트에 저장된 정보도 사라져 게임을 처음부터 시작해야 한다.

<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>


<!-- 전체 구성 ------------------------------------------>
## 전체 구성  
### 게임 시작
게임 시작 직후, 저장된 게임이 있는지 확인하고, 있으면 저장된 게임을 이어서 시작하고 없으면 새 게임을 시작한다. BeginPlay는 레벨이 오픈될 때마다 호출되므로, 정적 변수를 선언하여 게임을 처음 실행한 경우에만 플레이어에게 새 게임을 할 것인지, 아니면 저장된 게임을 이어서 할 것인지 묻도록 한다.
```C++
/// MyGameMode.cpp BeginPlay 중 일부 /////
//
// (1) 저장된 게임이 없으면 새 게임 시작
// (2) 저장된 게임이 있고 최초 플레이면 플레이어가 플레이 옵션 선택
// (3) 저장된 게임이 있고 두 번째 이후 플레이면 즉시 게임 로드
if (!UMySaveGame::HasSavedGame()) {
  PlayNewGame();  // (1)
} else if (bFirstPlay) {
  ShooterController->DisplayGameplayOptions();  // (2)
} else {
  LoadGame();  // (3)
}
```
새 게임을 시작하는 경우에는 기존 게임 정보를 삭제하고, 저장된 게임을 이어서 시작하는 경우에는 저장된 정보를 로드한다. 게임 로드를 위해 관리하는 정보는 크게 게임 모드(남은 목숨 수, 게임 플레이 시간 등), 플레이어(마지막 위치, 체력, 무기 상태 등), 몬스터(종류, 순찰 지점 등)에 대한 것이다. 정보가 많기 때문에 모든 코드를 게임 모드에 두는 것이 아니라, 플레이어와 몬스터의 정보는 각각의 코드에서 저장하고 로드하도록 구현했다. 또한, 게임의 저장은 로딩의 역과정이 되도록 대칭적으로 구현했다.
```C++
/// MyGameMode.cpp /////
void AMyGameMode::LoadGame() {
  bFirstPlay = false;
  UMySaveGame *LoadGameInstance = UMySaveGame::MakeLoadGameInstance();

  /// (A) 게임을 로드하는 부분 - SaveGame의 (B)와 대칭  ////////////////////////////
  if (UGameplayStatics::GetCurrentLevelName(this) != LoadGameInstance->Level) {
    UGameplayStatics::OpenLevel(this, FName(*LoadGameInstance->Level));
    return;
  }
  PlaySeconds = LoadGameInstance->PlaySeconds;
  KillCount   = LoadGameInstance->KillCount;
  PlayerLives = LoadGameInstance->PlayerLives;
  if (!LoadGameInstance->bPlayerStart) {
    RecreateEnemiesFromList(LoadGameInstance->EnemyProfiles);
  }
  Shooter->Load(LoadGameInstance->PlayerStats, LoadGameInstance->bPlayerStart);
  //////////////////////////////////////////////////////////////////////////////

  ShooterController->StartHUD();
}
```

<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>


<!-- 세부 구현 ------------------------------------------>
## 세부 구현
### 몬스터의 상태 변화
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229267668-b2ec49ca-6005-4d67-be92-ecd6a12a04e5.png" width="720" />

  _[몬스터의 상태 변화]_
</div>

게임에서 몬스터는 일곱 가지의 상태를 거칠 수 있다. 몬스터는 레벨에 배치될 때 **Spawned** 상태에서 시작하여, 게임 시작 직후 **Idle** 상태로 전이한다. 이후에는 별다른 일이 없으면 Idle 상태와 **Patrol** 상태를 오간다. 즉, 일정 시간이 경과하면 자동으로 Patrol 상태로 전이하고, 순찰 지점에 도착하면 다시 Idle 상태로 돌아가 일정 시간 동안 휴식을 취한다.

플레이어가 일정 반경 이내로 몬스터에 접근하면, 몬스터는 내부적으로 Tick 함수를 활성화하여 매 프레임마다 플레이어가 보이는지 확인한다. 플레이어가 보이는 순간부터 몬스터는 플레이어와 떨어진 거리에 따라 추격(**Chase**) 또는 공격(**Attack**)을 한다. 그러나, 플레이어가 충분히 멀리 도망치면 몬스터는 다시 Idle 상태로 진입하며, Tick 함수는 비활성화된다. 추가로, 몬스터가 플레이어에게 피해를 입으면 원거리에서도 추격을 시작할 수 있다. 몬스터의 체력이 바닥나면 **Dead** 상태가 되고, 일정 시간이 경과하면 **Sink** 상태가 된다. 몬스터가 충분히 아래로 내려가 더 이상 보이지 않게 되면, 몬스터는 월드에서 제거된다.
<br>

```C++
/// Enemy.cpp Tick 중 일부 /////
// 플레이어가 추격 범위 내에 있을 때 동작
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
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 몬스터 순찰 지점 등록
<div align="center">
  <img src="https://compote.slate.com/images/697b023b-64a5-49a0-8059-27b963453fb1.gif" width="720" />

  *[몬스터 순찰 지점 등록]*
</div>

몬스터는 살아있는 동안 추격 또는 공격 중이 아니라면, Idle 상태와 Patrol 상태를 오간다. Patrol 상태에서는 개발 단계에서 등록된 순찰 지점들을 차례로 이동한다. 이를 간편하게 등록하기 위해, 언리얼 엔진의 _MakeEditWidget_ 기능을 활용하였다. 게임 시작 시, 몬스터를 기준으로 상대적인 좌표로 등록되어 있던 순찰 지점들을 모두 월드 기준 좌표로 변환한다. 또한, 몬스터가 처음 생성된 위치를 순찰 지점으로 추가 등록하기 때문에 미리 등록해둔 순찰 지점이 N개라면, 모두 (N + 1)개의 순찰 지점을 차례로 방문하게 된다.

```C++
/// Enemy.cpp /////
// 시작 위치를 마지막 순찰 지점으로 추가 후 모든 순찰 지점을 월드 좌표로 변환
void AEnemy::ConvertPatrolPointsToWorld() {
  PatrolPoints.Add(FVector::ZeroVector);
  const FTransform &Transform = GetActorTransform();
  for (FVector &PatrolPoint : PatrolPoints) {
    PatrolPoint = UKismetMathLibrary::TransformLocation(Transform, PatrolPoint);
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 스태미나 소모로 체력 회복
스태미나는 항상 일정한 속도로 감소하며, 체력은 스태미나의 양에 비례하여 회복된다. 즉, 스태미나가 100%일 때 가장 빠른 속도로 회복되며, 스태미나가 바닥인 상태에서는 전혀 회복되지 않는다.
```C++
/// Shooter.cpp /////
// 스프링암 오프셋 보간이 필요하거나 스태미나가 0이 아닌 경우에만 활성화
void AShooter::Tick (float DeltaSeconds) {
  Super::Tick(DeltaSeconds);

  InterpSpringArmOffset(DeltaSeconds);
  Stamina->Decrease(NormalDecreaseRate * DeltaSeconds);
  RecoverHealth(MaxRecoveryRate * Stamina->GetRatio() * DeltaSeconds);

  if (ShouldDisableTick()) SetActorTickEnabled(false);
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 걷기-달리기 전환
플레이어가 달리는 경우에는 스태미나가 추가로 감소한다. 스태미나가 일정량 이상일 때에만 플레이어는 달릴 수 있으며, 스태미나가 일정 수준 이하로 떨어지면 더 이상 달릴 수 없게 된다.
```C++
/// LocomotionComponent.cpp /////
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

/// LocomotionComponent.cpp /////
// 달리는 동안 스태미나 양을 확인해야 하므로 틱 함수 활성화
void ULocomotionComponent::TryRun() {
  if (Stamina->GetValue() >= MinRunStamina) {
    Movement->MaxWalkSpeed = RunSpeed;
    SetComponentTickEnabled(true);
  }
}

/// LocomotionComponent.cpp /////
// 더 이상 스태미나 양을 확인할 필요가 없으므로 틱 함수 비활성화
void ULocomotionComponent::Walk() {
  Movement->MaxWalkSpeed = WalkSpeed;
  SetComponentTickEnabled(false);
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 무기 장착
아이템에는 총알(폭약 포함), 포션, 무기 등이 있다. 총알과 포션은 수량 제한 없이 얻을 수 있으며, 총알은 인벤토리에 추가되고 포션은 즉시 복용하여 체력이나 스태미나 등을 회복한다. 그러나, 무기는 이미 같은 종류의 무기를 장착한 경우가 있기 때문에, <u>해당 종류의 무기를 장착하고 있지 않은 경우에만 아이템을 획득한다. 만일 이미 장착 중이라면, 플레이어에게 메시지를 출력하여 교체 여부를 선택할 수 있도록 한다.</u>
```C++
/// Item.cpp /////
void AItem::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  switch (ItemType) {
    case EItemType::EIT_Ammo:   EquipAmmo(Shooter);       break;
    case EItemType::EIT_Weapon: TryEquipWeapon(Shooter);  break;
    case EItemType::EIT_Potion: ApplyPotion(Shooter);     break;
  }
}

/// InventoryComponent.cpp /////
// (1) 같은 타입의 무기를 이미 보유 중이면 교체 방법을 설명하는 메시지 출력
// (2) 같은 타입의 무기를 보유 중이지 않다면 즉시 무기 장착
void UInventoryComponent::TryEquipWeapon(class AItem *Item) {
  const auto &[Type, Name] = ExtractTypeAndName(Item->GetWeaponBP());
  if (AWeapon *OldWeapon = Weapons[Type]) {
    LOG("%s(을)를 %s(으)로 교체하려면 E키를 누르세요.",
        *OldWeapon->GetWeaponName(), *Name);  // (1)
    OverlappingItem = Item;
  } else {
    EquipWeapon(Item);  // (2)
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 주기운동 컴포넌트 (UPeriodicalMovementComponent)
게임 내에서는 주기적인 움직임이 필요한 경우가 많다. 예를 들면, 아이템이 떠다니거나 지속적으로 회전하거나, 지형지물이 왔다갔다 하여 플레이어가 발판으로 이용하거나, 부비트랩이 움직이면서 플레이어에게 위협이 되어야 하는 등 다양한 상황이 있다. 이를 위해 **(1)병진운동**과 **(2)회전운동**을 하나의 주기운동 컴포넌트로 묶어 여러 액터가 공통으로 사용할 수 있도록 했다.
```C++
/// PeriodicalMovementComponent.cpp /////
void UPeriodicalMovementComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  if (bShouldTranslate) Translate();  // 병진운동
  if (bShouldRotate) Rotate(DeltaTime);  // 회전운동
}
```
특히, 병진운동은 간단한 벡터 수학을 응용하여 아래와 같은 원리로 구현할 수 있었다.
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229274222-66a1fd1f-0564-4d1e-93ec-cd50855c514e.png" width="360" />

  _[내분 벡터의 계산]_
  <br>
</div>

두 개의 끝점 사이에 위치한 점의 좌표는 <u>*내분점 공식*</u>을 이용하여 구한다. 내분 계수 𝛼의 값을 시간에 따라 적절히 변경하면, 내분점이 두 점 사이를 왕복하도록 할 수 있다.  
<br>

<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229275229-f87eb1ca-30f2-476d-ba70-13d9d55edd16.png" width="360" />

  _[𝛼 = 𝛼(t) 함수의 개형]_
</div>

위 그래프는 시간에 따른 내분 계수 𝛼의 변화를 나타낸 것이다. 이와 같이 내분 계수를 변화시키면, 액터가 양 끝점에서 잠시 멈추었다가 왕복하기를 반복하는 것이 가능하다. 그래프의 개형을 결정짓는 두 매개변수(t<sub>1</sub>, t<sub>2</sub>)는 **UPROPERTY**로 선언하여 개발 과정에서 변경할 수 있도록 하였다.  

```C++
/// PeriodicalMovementComponent.h 중 일부 /////
private:
  /** 시작 지점에서 반환점까지 이동하는 데에 걸리는 시간. */
  UPROPERTY(EditAnywhere, Category = "Config|Translation")
  float TravelTime;
  /** 시작 지점과 반환점에서 머무르는 시간. */
  UPROPERTY(EditAnywhere, Category = "Config|Translation")
  float WaitTime;

/// PeriodicalMovementComponent.cpp /////
// 내분점 공식으로 위치 계산
void UPeriodicalMovementComponent::Translate() {
  float Alpha = GetAlpha(GetModuloTime());
  FVector NextLocation = Alpha * TurningPoint + (1 - Alpha) * InitialPoint;
  GetOwner()->SetActorLocation(NextLocation);
}

/// PeriodicalMovementComponent.cpp /////
// 내분계수 𝛼의 변화를 구현한 코드
float UPeriodicalMovementComponent::GetAlpha(float Time) const {
  if (Time < TravelTime) return GetAlphaBeforeTravelTime(Time);
  if (Time < TravelTime + WaitTime) return 1.f;
  if (Time > 2 * TravelTime + WaitTime) return 0.f;

  Time -= TravelTime + WaitTime;
  return 1.f - GetAlphaBeforeTravelTime(Time);
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 폭약 (AExplosive)
폭약은 **부비트랩**(booby trap)과 플레이어가 사용 가능한 **폭탄**(grenade, rocket) 등 두 종류의 액터를 구현하는 데에 사용된다. 특히, 후자는 가속 컴포넌트(**UAccelMovementComponent**)의 도움을 받아 런처(launcher)에 의해 발사된다. 폭약은 다른 액터와 충돌 시 폭발하며, 일정 반경 내에 있는 액터(*플레이어와 몬스터 모두*)들에게 데미지를 가한다.  

```C++
/// Explosive.cpp /////
// 오버로드 함수들의 베이스 함수
// (*) 힛 이벤트와 타이머에 의해 중복으로 폭파하지 않도록 방지
void AExplosive::Explode(const FVector &FXDirection) {
  if (!bExploded) {
    bExploded = true;  // (*)
    SpawnFXAndSound(FXDirection);
    ApplyDamageToOverlappingActors();
    DestroyAfterDelay();
  }
}

/// Explosive.cpp /////
void AExplosive::ApplyDamageToOverlappingActors() {
  TArray<AActor *> Victims;
  KillingRange->GetOverlappingActors(Victims, APawn::StaticClass());
  for (AActor *Victim : Victims) {
    ApplyDamage(Victim);
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 포털 (APortal)
플레이어가 포털과 오버랩 하는 경우, (1)_다음 레벨로 이동_ 하거나, (2)_게임이 종료_(승리)된다. **UPROPERTY**로 변수를 선언하여 둘 중 어느 역할을 수행할지 설정 가능하다. 논리적으로 모든 레벨은 하나 이상의 포털을 가지고 있어야 한다. 따라서, 게임 시작 시 레벨에 포털이 있는지 확인하고, 없다면 이를 개발자에게 알리도록 하여 혹시 모를 버그를 미연에 방지하였다.

```C++
/// Portal.h 중 일부 /////
private:
  /** 다음 레벨 이름. */
  UPROPERTY(EditAnywhere, Category = "Config")
  FString NextLevel;
  /** 체크하면 마지막 포탈이 됨. 오버랩 시 모든 레벨을 클리어 하고 게임 승리. */
  UPROPERTY(EditAnywhere, Category = "Config")
  bool bEndOfGame;

/// Portal.cpp /////
void APortal::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  if (bEndOfGame) {
    GameMode->ClearGame();  // 게임 승리
  } else {
    GameMode->ClearLevel(NextLevel);  // 다음 레벨로 이동
  }
}

/// MyGameMode.cpp /////
// 레벨마다 포탈이 반드시 존재하도록 강제함
void AMyGameMode::InitializePortal() {
  TSubclassOf<AActor> PortalClass = APortal::StaticClass();
  Portal = Cast<APortal>(UGameplayStatics::GetActorOfClass(this, PortalClass));
  if (!Portal) {
    UE_LOG(LogTemp, Fatal, TEXT("현재 레벨에 포탈이 존재하지 않습니다."));
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 오버랩 베이스 클래스 (AShooterOverlap)
_아이템_, _포털_, 그리고 _세이브 포인트_ 는 모두 플레이어와 오버랩 했을 때 특정 기능을 수행하며, 다른 액터와는 충돌하지 않아야 한다. 이를 위해서, 이 세 클래스는 **AShooterOverlap**이라는 베이스 클래스를 공유하며, 베이스 클래스에서는 오버랩 했을 때 상호작용하는 액터를 확인한다. 따라서, 서브 클래스에서는 별다른 확인 없이 플레이어와 오버랩 했음을 보장 받을 수 있다.  

```C++
/// ShooterOverlap.cpp AShooterOverlap 중 일부 /////
// 루트 콜리전 컴포넌트 생성: 폰 채널과만 오버랩
OverlapSphere = CreateDefaultSubobject<
    USphereComponent>(TEXT("OverlapSphere"));
SetRootComponent(OverlapSphere);
OverlapSphere->SetCollisionProfileName(TEXT("IgnoreAllButPawn"));
```

```C++
/// ShooterOverlap.cpp /////
// 오버랩 한 액터가 플레이어인 경우에만 OnShooterOverlap 함수를 호출하므로, 함수 안에서 Shooter가 플레이어임이 보장됨
// OnShooterOverlap은 가상함수
void AShooterOverlap::OnSphereBeginOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, 
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult &SweepResult) {
  if (AShooter *Shooter = Cast<AShooter>(OtherActor)) {
    OnShooterOverlap(Shooter);
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

## 추가로 구현하고 싶은 기능들
- 총기 발사 시 카메라 진동
- 저격 소총 줌 기능
- 몬스터 스킬 사용
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>
