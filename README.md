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
  - [저장 및 불러오기](#저장-및-불러오기)
  - [적의 죽음](#적의-죽음)
  - [플레이어의 죽음](#플레이어의-죽음)
  - [승패 결정](#승패-결정)
- 세부 구현
  - [적의 상태 변화](#적의-상태-변화)
  - [적 순찰 지점 등록](#적-순찰-지점-등록)
  - [스태미나 소모로 체력 회복](#스태미나-소모로-체력-회복)
  - [걷기-달리기 전환](#걷기-달리기-전환)
  - [무기 체계](#무기-체계)
    - [나이프](#나이프-aknife)
    - [화기](#화기-afirearm)
      - [총](#총-agun)
      - [런처](#런처-alauncher)
  - [무기 장착](#무기-장착)
  - [주기운동 컴포넌트](#주기운동-컴포넌트-uperiodicalmovementcomponent)
  - [폭약](#폭약-aexplosive)
  - [포털](#포털-aportal)
  - [오버랩 베이스 클래스](#오버랩-베이스-클래스-ashooteroverlap)
- [미포함](#미포함)
<br>

<!-- 게임 소개 ------------------------------------------>
## 게임 소개
이 게임은 PC에서 실행되며, 1인용 **TPS**(3인칭 슈팅) 및 **사냥** 게임이다. 맵에는 다수의 적가 존재하며, 플레이어는 이들을 처치하고 레벨을 클리어하여 다음 레벨로 진행한다. 모든 레벨을 클리어하면 플레이어가 승리한다. 플레이어는 적의 공격을 받을 수 있으며, 최대 세 개의 목숨이 부여된다. 모든 목숨을 소진하면 플레이어는 패배한다.

플레이어는 **맨손, 단검, 각종 총기류 및 폭약 등**을 사용하여 적에게 피해를 입힐 수 있다. 그러나, **적나 각종 장애물**(불, 독가스, 부비트랩)에 의해 플레이어가 피해를 입을 수도 있다. 플레이어가 피해를 입으면 체력이 감소하지만, 체력 포션을 사용하여 회복할 수 있다. 또한, 스태미나 상태가 관리되며 일정 수준 이상이면 달리기가 가능하고, 스태미나가 채워진 양에 비례하여 체력이 지속적으로 회복된다. 단, 달리는 동안 체력은 회복되지 않는다.

총기류는 총알이 있어야만 사용할 수 있으며, 총알이 바닥나면 총기는 발사되지 않는다. 무기, 총알 및 포션 등을 아이템으로 획득할 수 있다. 각 레벨에는 여러 개의 **세이브 포인트**가 배치되어 있으며, 이 포인트와 겹치면 게임 진행 상황이 저장된다. 따라서, 다음 플레이 시 이전에 저장한 지점에서 게임을 이어나갈 수 있다. 그러나, 모든 목숨을 소진하면 세이브 포인트에 저장된 정보도 사라져 게임을 처음부터 시작해야 한다.

<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>


<!-- 전체 구성 ------------------------------------------>
## 전체 구성  
### 저장 및 불러오기
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
새 게임을 시작하는 경우에는 기존 게임 정보를 삭제하고, 저장된 게임을 이어서 시작하는 경우에는 저장된 정보를 로드한다. 게임 로드를 위해 관리하는 정보는 크게 게임 모드(남은 목숨 수, 게임 플레이 시간 등), 플레이어(마지막 위치, 체력, 무기 상태 등), 적(종류, 순찰 지점 등)에 대한 것이다. 정보가 많기 때문에 모든 코드를 게임 모드에 두는 것이 아니라, 플레이어와 적의 정보는 각각의 코드에서 저장하고 로드하도록 구현했다. 또한, 게임의 저장은 로딩의 역과정이 되도록 대칭적으로 구현했다.
```C++
/// MyGameMode.cpp 중 일부 /////
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

### 적의 죽음
게임은 한 개 이상의 레벨로 구성되고, 레벨마다 하나 이상의 적가 있다. 레벨을 클리어 하기 위해서는 레벨에 존재하는 모든 적를 제거해야 한다. 적는 체력이 바닥나면 내부적으로 게임 모드에 처리를 위임하고, 게임 모드는 레벨에 존재하는 적의 수를 하나씩 감소시킨다. 만일 모든 적가 제거되면 레벨에 존재하는 포털을 활성화 하여 플레이어가 상호작용할 수 있도록 한다.
```C++
/// MyGameMode.cpp 중 일부 /////
// (1) 적를 죽인 것이 플레이어인 경우에만 킬 수 증가
// (2) 적를 모두 처치했으면 포탈을 작동시킴
void AMyGameMode::HandleEnemyDeath(AController *EventInstigator) {
  EnemyCount--;
  if (EventInstigator == ShooterController) {
    KillCount++;  // (1)
  }
  if (!EnemyCount) {
    Portal->SetActivated(true);  // (2)
    LOG("적을 모두 처치했습니다! 포탈로 이동하세요.");
  }
  if (HUD) HUD->UpdateEnemyCount();
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 플레이어의 죽음
플레이어는 3개의 목숨을 가진 채로 게임을 시작한다. 적 또는 장애물의 공격을 받아 체력이 감소하다가 마침내 0이 되면, 내부적으로 게임 모드에 처리를 위임한다. 게임 모드에서는 플레이어의 목숨 수를 1만큼 감소시키고, 목숨이 남아있는 경우에는 저장된 게임이 있는 경우에 한하여 목숨 수를 1 작은 값으로 업데이트 하여 저장한 후 현재 레벨을 다시 플레이 하도록 한다. 만일 저장된 게임이 없다면 어차피 새 게임을 시작하게 되므로 목숨 수를 변경하지 않아야 한다. 플레이어가 가지고 있는 목숨을 모두 소모하게 되면 게임에 패배하게 된다.
```C++
// (*) 게임을 처음부터 시작하는데 목숨 수가 감소되어 있으면 안 되므로 저장된 게임이 있는 경우에만 목숨 수 감소
void AMyGameMode::HandlePlayerDeath() {
  if (--PlayerLives) {
    // 아직 목숨이 남았으면 감소한 목숨 수를 저장 후 현재 레벨 다시 플레이
    if (UMySaveGame::HasSavedGame()) {
      UMySaveGame *LoadGameInstance = UMySaveGame::MakeLoadGameInstance();
      LoadGameInstance->PlayerLives = PlayerLives;
      UMySaveGame::SaveGame(LoadGameInstance);
    }
    UGameplayStatics::OpenLevel(
        this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
  } else {
    // 목숨이 남아있지 않으면 게임 삭제 후 게임 패배 스크린을 보임
    UMySaveGame::DeleteGame();
    ShooterController->DisplayLoseScreen();
    if (HUD) HUD->UpdatePlayerLives();
  }
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 승패 결정
레벨에 존재하는 에너미를 모두 제거하면 포털이 활성화 된다. 포털을 통해 다음 레벨로 이동하거나 마지막 레벨의 경우에는 게임을 클리어 하고 승리하게 된다(**DisplayWinScreen**). 플레이어가 죽어서 주어진 3개의 목숨을 모두 소모하면 게임에서 패배한다(**DisplayLoseScreen**).
```C++
/// MyGameMode.cpp 중 일부
// 마지막 레벨까지 클리어 했을 때 호출되는 점에서 ClearLevel과 다름
void AMyGameMode::ClearGame() {
  UMySaveGame::DeleteGame();
  ShooterController->DisplayWinScreen();
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

<!-- 세부 구현 ------------------------------------------>
## 세부 구현
### 적의 상태 변화
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229267668-b2ec49ca-6005-4d67-be92-ecd6a12a04e5.png" width="720" />

  _[적의 상태 변화]_
</div>

게임에서 적는 일곱 가지의 상태를 거칠 수 있다. 적는 레벨에 배치될 때 **Spawned** 상태에서 시작하여, 게임 시작 직후 **Idle** 상태로 전이한다. 이후에는 별다른 일이 없으면 Idle 상태와 **Patrol** 상태를 오간다. 즉, 일정 시간이 경과하면 자동으로 Patrol 상태로 전이하고, 순찰 지점에 도착하면 다시 Idle 상태로 돌아가 일정 시간 동안 휴식을 취한다.

플레이어가 일정 반경 이내로 적에 접근하면, 적는 내부적으로 Tick 함수를 활성화하여 매 프레임마다 플레이어가 보이는지 확인한다. 플레이어가 보이는 순간부터 적는 플레이어와 떨어진 거리에 따라 추격(**Chase**) 또는 공격(**Attack**)을 한다. 그러나, 플레이어가 충분히 멀리 도망치면 적는 다시 Idle 상태로 진입하며, Tick 함수는 비활성화된다. 추가로, 적가 플레이어에게 피해를 입으면 원거리에서도 추격을 시작할 수 있다. 적의 체력이 바닥나면 **Dead** 상태가 되고, 일정 시간이 경과하면 **Sink** 상태가 된다. 적가 충분히 아래로 내려가 더 이상 보이지 않게 되면, 적는 월드에서 제거된다.
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

### 적 순찰 지점 등록
<div align="center">
  <img src="https://compote.slate.com/images/697b023b-64a5-49a0-8059-27b963453fb1.gif" width="720" />

  *[적 순찰 지점 등록]*
</div>

적는 살아있는 동안 추격 또는 공격 중이 아니라면, Idle 상태와 Patrol 상태를 오간다. Patrol 상태에서는 개발 단계에서 등록된 순찰 지점들을 차례로 이동한다. 이를 간편하게 등록하기 위해, 언리얼 엔진의 _MakeEditWidget_ 기능을 활용하였다. 게임 시작 시, 적를 기준으로 상대적인 좌표로 등록되어 있던 순찰 지점들을 모두 월드 기준 좌표로 변환한다. 또한, 적가 처음 생성된 위치를 순찰 지점으로 추가 등록하기 때문에 미리 등록해둔 순찰 지점이 N개라면, 모두 (N + 1)개의 순찰 지점을 차례로 방문하게 된다.

```C++
/// Enemy.cpp 중 일부 /////
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
/// Shooter.cpp 중 일부 /////
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
/// LocomotionComponent.cpp 중 일부 /////
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
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 무기 체계
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229324860-14f4e5b0-4e1f-4849-a9db-8381f9f14baa.png" width="720" />

  _[무기 상속 관계도]_
</div>

위와 같은 상속 체계를 갖는 무기는, 기본적으로 공격 기능을 갖고 있는 **Weapon** 클래스를 상속한다. 이를 오버라이드하면서, 무기마다 고유한 특화된 공격 기능을 구현할 수 있다.

그러나, 무기를 장착하고 있더라도 항상 공격을 수행할 수 있는 것은 아니다. 이미 공격 중인 상황이거나, 다른 종류의 무기로 교체하고 있거나, 칼을 휘두르고 있는 등의 상황이 있을 수 있다. 총기의 경우 총알이 없는 경우도 해당한다. 따라서, 플레이어가 왼쪽 마우스를 클릭하면 항상 공격이 수행되는 것은 아니며, 공격이 가능한 상황인지 판단하여 조건이 만족되는 경우에만 공격이 수행되도록 로직이 구성되어야 한다.

```C++
/// Weapon.h 중 일부 /////
public:
  /** 공격 시도. */
  void TryAttack();

protected:
  /** 공격 시도 시 실제로 공격을 수행하는 조건을 정의. */
  virtual bool CanAttack();
  /** 공격 시도 시 CanAttack이 true를 반환하는 상황에 호출. */
  virtual void Attack();
  /** 공격 시도 시 CanAttack이 false를 반환하는 상황에 호출. */
  virtual void OnAttackFailure();
```

```C++
/// Weapon.cpp 중 일부 /////
void AWeapon::TryAttack() {
  if (CanAttack()) {
    Attack();
  } else {
    OnAttackFailure();
  }
}

bool AWeapon::CanAttack() { return !bAttacking; }

void AWeapon::Attack() {
  // 공격하는 동안 bAttacking을 true로 만듦
  bAttacking = true;
  GetWorldTimerManager().SetTimer(
      EndAttackTimer, this, &AWeapon::OnAttackEnd, AttackWaitTime);

  // 애니메이션|사운드 재생
  UShooterAnim *ShooterAnim =
      Cast<UShooterAnim>(Shooter->GetMesh()->GetAnimInstance());
  ShooterAnim->PlayAttackMontage();
  UGameplayStatics::PlaySound2D(this, AttackSound);
}

void AWeapon::OnAttackFailure() {}
```
플레이어가 무기를 장착한 상태에서 왼쪽 마우스를 클릭하면, 장착한 무기의 **TryAttack** 메서드가 호출된다. 이때, **CanAttack** 메서드가 true를 반환하는 상황에서만 실제 공격이 수행되며(**Attack**), false를 반환하는 상황에서는 **OnAttackFailure**가 호출된다. 일반적으로, 아직 공격이 끝나지 않은 상황에서 다시 공격할 수는 없으므로, CanAttack은 공격 중이지 않을 때 true를 반환하도록 구현했다.

또한, Attack 메서드 실행 시 _**bAttacking**_ 변수를 true로 변경한 후 공격이 끝나면 다시 false로 되돌려, CanAttack이 실제 공격 중인지 여부를 반영할 수 있도록 했다. 베이스 클래스에서는 공격 시도가 실패할 때 따로 수행할 기능이 없으므로 OnAttackFailure 메서드의 구현이 비어있다. 하지만 CanAttack, Attack, OnAttackFailure 메서드는 모두 virtual로 선언되어 있으므로, 베이스 클래스 기능의 확장이 필요하다면 이를 오버라이드할 수 있다. 예를 들어, <u>_Firearm 클래스에서 CanAttack 메서드는 총알이 없는 경우에 false를 반환하며, OnAttackFailure 메서드에서 빈 탄창 사운드를 재생하도록 오버라이드 되어있다_</u>. 따라서, 총알이 없는 경우에 총기를 사용한 공격을 시도하면, 빈 탄창 사운드만 재생되고 총알은 발사되지 않는다.
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 나이프 (AKnife)
나이프를 사용한 공격 시 단순히 칼날에 부착된 콜리전을 일시적으로 활성화 한다.
```C++
/// Knife.cpp 중 일부
// @0.23[s]: 오버랩 활성화, @0.84[s]: 오버랩 비활성화
void AKnife::Attack() {
  Super::Attack();
  GetWorldTimerManager().SetTimer(
      EnableTimer, this, &AKnife::EnableBladeCollision, 0.23f);
  GetWorldTimerManager().SetTimer(
      DisableTimer, this, &AKnife::DisableBladeCollision, 0.84f);
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 화기 (AFirearm)
총과 런처는 모두 _발사할 대상_(총알 혹은 폭약)이 필요하다. 따라서, 장전(_LoadAmmos_)과 같은 기능을 공유하고 있으며, 장전되어 있지 않다면 공격할 수 없다. 이들 기능은 **AFirearm** 클래스에 정의되어 있으며, 총과 런처는 이를 상속한다. 이 둘은 공격하는 방식에 있어서 약간의 차이를 보이는데, 이는 각각의 클래스에서 오버라이딩을 통해 구현되었다.
```C++
/// Firearm.cpp 중 일부
// 장전된 총알이 없으면 공격 불가
bool AFirearm::CanAttack() { return Super::CanAttack() && LoadedAmmos; }

void AFirearm::Attack() {
	Super::Attack();
	LoadedAmmos--;  // 총알 감소
	if (HUD) HUD->UpdateLoadedAmmos(LoadedAmmos);

	// 발사 이펙트
	UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh, FName("MuzzleFlash"));
	UGameplayStatics::SpawnEmitterAttached(
			MuzzleRearFX, Mesh, FName("MuzzleFlashRear"));
}

// 빈 탄창 사운드 재생
void AFirearm::OnAttackFailure() {
	if (!LoadedAmmos) {
		UGameplayStatics::PlaySound2D(this, NoAmmoSound);
	}
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 총 (AGun)
총이 런처와 가장 다른 점은 공격 시 실제 총알이 발사되는 것이 아니라, 라인 트레이스(**line trace**) 검사를 통해 힛(**hit**)을 검사한다는 점이다. 검사 결과, 힛 대상이 적인 경우 데미지를 가하고, 그 외 지점에 맞은 경우에는 힛 이펙트를 발생시킨다(ex. 돌이 튀기는 효과).
```C++
/// Gun.cpp 중 일부 /////
void AGun::Attack() {
  Super::Attack();
	EjectAmmo();  // 탄피 배출

	// 라인 트레이스 수행
	if (FHitResult HitResult; LineTrace(HitResult)) {
		if (AEnemy *HitEnemy = Cast<AEnemy>(HitResult.GetActor())) {
			// (1) 적가 맞은 경우
			SpawnHitFX(HitEnemy->GetDamagedFX(), HitResult);
			ApplyDamage(HitEnemy);
		} else { // (2) 다른 곳에 맞은 경우
			SpawnHitFX(HitFX, HitResult);
		}
	}
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 런처 (ALauncher)
총이 라인 트레이스를 통해 공격을 수행한다면, 런처는 실제 폭약([AExplosive](#폭약-aexplosive))을 발사하고, 폭약의 뇌관에 콜리전이 발생하면 즉시 폭발하여 일정 반경 내의 액터에 데미지를 가한다.
```C++
/// Launcher.cpp 중 일부 /////
// 카메라 방향으로 폭약의 머리 부분이 오도록 소켓 위치에 폭약 생성
void ALauncher::Attack() {
  Super::Attack();
  FVector SocketLocation = Mesh->GetSocketLocation(TEXT("AmmoSocket"));
  FRotator Rotation = UKismetMathLibrary::MakeRotFromZ(
      Shooter->GetControlRotation().Vector());
  
  // 런처는 단순히 폭약을 생성할 뿐이고,
  // 발사 기능은 액터에 부착된 UAccelMovementComponent에서 구현
  AExplosive *Explosive = GetWorld()->SpawnActor<
      AExplosive>(ExplosiveBP, SocketLocation, Rotation);
  Explosive->Instigator = Shooter->GetController();
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
/// Item.cpp 중 일부 /////
void AItem::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  switch (ItemType) {
    case EItemType::EIT_Ammo:   EquipAmmo(Shooter);       break;
    case EItemType::EIT_Weapon: TryEquipWeapon(Shooter);  break;
    case EItemType::EIT_Potion: ApplyPotion(Shooter);     break;
  }
}
```
```C++
/// InventoryComponent.cpp 중 일부 /////
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
/// PeriodicalMovementComponent.cpp 중 일부 /////
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
```
```C++
/// PeriodicalMovementComponent.cpp 중 일부 /////
// 내분점 공식으로 위치 계산
void UPeriodicalMovementComponent::Translate() {
  float Alpha = GetAlpha(GetModuloTime());
  FVector NextLocation = Alpha * TurningPoint + (1 - Alpha) * InitialPoint;
  GetOwner()->SetActorLocation(NextLocation);
}

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
폭약은 **부비트랩**(booby trap)과 플레이어가 사용 가능한 **폭탄**(grenade, rocket) 등 두 종류의 액터를 구현하는 데에 사용된다. 특히, 후자는 가속 컴포넌트(**UAccelMovementComponent**)의 도움을 받아 런처(launcher)에 의해 발사된다. 폭약은 다른 액터와 충돌 시 폭발하며, 일정 반경 내에 있는 액터(*플레이어와 적 모두*)들에게 데미지를 가한다.  

```C++
/// Explosive.cpp 중 일부 /////
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
```
```C++
/// Portal.cpp 중 일부 /////
void APortal::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  if (bEndOfGame) {
    GameMode->ClearGame();  // 게임 승리
  } else {
    GameMode->ClearLevel(NextLevel);  // 다음 레벨로 이동
  }
}
```
```C++
/// MyGameMode.cpp 중 일부 /////
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
/// ShooterOverlap.cpp 중 일부 /////
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

## 미포함
- 총기 발사 시 카메라 진동
- 저격 소총 줌 기능
- 적 스킬 사용
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>
