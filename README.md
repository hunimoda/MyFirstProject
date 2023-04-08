_<div align="right">Last update: 2023. 4. 8.</div>_

<!-- 제목 ---------------------------------------------->
# MyFirstProject
<br>
<div align="center">
  <img src="http://via.placeholder.com/640x480" width="720" />

  *[플레이 영상(일부) - [전체 영상 링크](https://www.google.com/webhp?hl=en)]*
</div>
<br>

<!-- 개요 ---------------------------------------------->
---
## 개요
- 프로젝트명: **MyFirstProject**
- 수행 기간: **2023. 1. ~ 2023. 2. (약 두 달간)**
- 참여 인원: **1명**
- 게임 분류: **PC 게임, TPS + 사냥(?)**
- 사용 기술: **C++, Unreal Engine v5.0.3**  
<br>

<!-- 목차 ---------------------------------------------->
---
## 목차
- [게임 소개](#게임-소개)
- 전체 구성 (AMyGameMode)
  - [저장 및 불러오기 (Load|SaveGame)](#저장-및-불러오기-loadsavegame)
  - [적이 죽었을 때 (HandleEnemyDeath) > 勝](#적이-죽었을-때-handleenemydeath--勝)
  - [플레이어가 죽었을 때 (HandlePlayerDeath) > 敗](#플레이어가-죽었을-때-handleplayerdeath--敗)
- 세부 구현
  - 적 (AEnemy)
    - [적의 상태(EBehaviorStatus) 변화](#적의-상태ebehaviorstatus-변화)
    - [적 순찰 지점(PatrolPoints) 등록](#적-순찰-지점patrolpoints-등록)
  - [무기 체계 (AWeapon)](#무기-체계-aweapon)
    - [나이프 (AKnife)](#나이프-aknife)
    - [화기 (AFirearm)](#화기-afirearm)
      - [총 (AGun)](#총-agun)
      - [런처 (ALauncher)](#런처-alauncher)
  - 주요 컴포넌트의 핵심 기능
    - [왕복 병진운동 (UPeriodicalMovementComponent)](#왕복-병진운동-uperiodicalmovementcomponent)
    - [가속 (UAccelMovementComponent)](#가속-uaccelmovementcomponent)
    - [걷기-달리기 전환 (ULocomotionComponent)](#걷기-달리기-전환-ulocomotioncomponent)
    - [무기 장착 (UInventoryComponent)](#무기-장착-uinventorycomponent)
  - 그 외
    - [폭약 (AExplosive)](#폭약-aexplosive)
    - [포털 (APortal)](#포털-aportal)
    - [플레이어 오버랩 (AShooterOverlap)](#플레이어-오버랩-ashooteroverlap)
    - [체력 회복 (Stamina)](#체력-회복-stamina)
- [구현 예정](#구현-예정)
<br>

<!-- 게임 소개 ------------------------------------------>
---
## 게임 소개
이 게임은 **PC**에서 실행되며, 1인용 **TPS**(3인칭 슈팅) 및 **사냥** 게임이다. 맵에는 다수의 적이 존재하며, 플레이어는 이들을 처치하고 레벨을 클리어하여 다음 레벨로 진행한다. 모든 레벨을 클리어하면 플레이어가 승리한다. 플레이어는 적의 공격을 받을 수 있으며, 최대 세 개의 목숨이 부여된다. 모든 목숨을 소진하면 플레이어는 패배한다.

플레이어는 **맨손, 단검, 각종 총기류 및 폭약** 등을 사용하여 적에게 피해를 입힐 수 있다. 그러나, **적이나 각종 장애물**(불, 독가스, 부비트랩 등)에 의해 플레이어가 피해를 입을 수도 있다. 플레이어가 피해를 입으면 체력이 감소하지만, 포션을 사용하여 체력을 회복할 수 있다. 또한, **스태미나** 상태가 관리되며 일정 수준 이상이면 달리기가 가능하고, 스태미나가 채워진 양에 비례하여 체력이 지속적으로 회복된다. 단, 달리는 동안 체력은 회복되지 않는다.

총기류는 총알이 있어야만 사용할 수 있으며, 총알이 바닥나면 총기는 발사되지 않는다. 총알은 무기나 포션과 같이 **아이템**으로부터 얻을 수 있다. 한편, 각 레벨에는 여러 개의 **세이브 포인트**가 배치되어 있으며, 플레이어가 이와 오버랩하면 게임 진행 상황이 저장된다. 따라서, 다음 플레이 시 이전에 저장한 지점에서 게임을 이어나갈 수 있다. 그러나, 모든 목숨을 소진하면 세이브 포인트에 저장된 정보도 사라져 게임을 처음부터 시작해야 한다.

<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>


<!-- 전체 구성 ------------------------------------------>
---
## 전체 구성  
### 저장 및 불러오기 (Load|SaveGame)
게임 시작 직후, 저장된 게임이 있는지 확인하고, 있으면 저장된 게임을 이어서 시작하고 없으면 새 게임을 시작한다. BeginPlay는 레벨이 오픈될 때마다 호출되므로, 정적 변수(**bFirstPlay**)를 선언하여 게임을 처음 실행한 경우에만 플레이어에게 새 게임을 할 것인지, 아니면 저장된 게임을 이어서 할 것인지 묻도록 한다.
```C++
/// MyGameMode.cpp BeginPlay 중 일부 /////

// ...
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
// ...
```
새 게임을 시작하는 경우에는 기존 게임 정보를 삭제하고, 저장된 게임을 이어서 시작하는 경우에는 저장된 정보를 로드한다. 게임 로드를 위해 관리하는 정보는 _<u>게임 모드</u>_(남은 목숨 수, 게임 플레이 시간 등), _<u>플레이어</u>_(마지막 위치, 체력, 무기 상태 등), _<u>적</u>_(종류, 순찰 지점 등)에 대한 것이다. 관리하는 정보가 많기 때문에 모든 로직을 게임 모드에서 수행하지 않고, 플레이어와 적의 정보는 각각의 클래스에서 로드하도록 구현했다. 또한, 게임의 저장은 로딩의 역과정이 되도록 대칭적으로 구현했으며, 이 역시 플레이어와 적 클래스에서 로직을 분담하게 만들었다.
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

void AMyGameMode::SaveGame(const FString &Level, bool bPlayerStart) {
  UMySaveGame *SaveGameInstance = UMySaveGame::MakeSaveGameInstance();

  /// (B) 게임을 저장하는 부분 - LoadGame의 (A)와 대칭  ////////////////////////////
  SaveGameInstance->bPlayerStart = bPlayerStart;
  SaveGameInstance->Level        = Level;
  SaveGameInstance->PlaySeconds  = PlaySeconds;
  SaveGameInstance->KillCount    = KillCount;
  SaveGameInstance->PlayerLives  = PlayerLives;
  PopulateListWithEnemies(SaveGameInstance->EnemyProfiles);
  Shooter->Save(SaveGameInstance->PlayerStats);
  //////////////////////////////////////////////////////////////////////////////

  UMySaveGame::SaveGame(SaveGameInstance);
}
```

<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

### 적이 죽었을 때 (HandleEnemyDeath) > 勝
게임은 레벨이 하나 이상 포함되며, 각 레벨마다 하나 이상의 적이 등장한다. 레벨을 클리어하려면 해당 레벨의 모든 적을 처치해야 한다. 적의 체력이 0이 되면, 게임 모드가 내부적으로 레벨에 있는 적의 수를 하나씩 줄인다. 만약 레벨에 등장하는 모든 적을 처치하면 [포털](#포털-aportal)이 활성화되고, 플레이어가 이를 통해 다음 레벨로 이동할 수 있다. 마지막 레벨의 경우에는 게임을 클리어하고 승리하게 된다.
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

### 플레이어가 죽었을 때 (HandlePlayerDeath) > 敗
게임을 시작할 때 플레이어는 **3개**의 목숨을 가진다. 플레이어는 적 또는 장애물의 공격을 받으면 체력이 감소하고, 체력이 0이 되면 죽게 된다. 이때, 게임 모드에서는 플레이어의 목숨 수를 차감하고, 남아있는 목숨이 없다면 게임에서 패배하게 된다. 목숨이 남아있는 경우에는 저장된 게임이 있는지에 따라 처리가 달라지게 된다. 어느 경우이든 현재 레벨을 다시 플레이 하도록 해야 하지만, 저장된 게임이 있다면 목숨 수가 차감되어야 하고, 없다면 목숨 수가 그대로여야 한다. 저장된 게임이 없다면 어차피 새 게임을 시작하게 되므로, 목숨 수가 줄어드는 것이 불이익이 되기 때문이다.
```C++
/// MyGameMode.cpp 중 일부 /////

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

<!-- 세부 구현 ------------------------------------------>
---
## 세부 구현
### 적 (AEnemy)
#### 적의 상태(EBehaviorStatus) 변화
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229267668-b2ec49ca-6005-4d67-be92-ecd6a12a04e5.png" width="720" />

  _[적의 상태 변화]_
</div>

게임 중에 적은 7개의 가능한 상태 가운데 어느 하나에 놓여있다. 레벨에 배치될 때에는 **Spawned** 상태에서 시작해서, 게임 시작 직후에 **Idle** 상태로 전이한다. 이후에는 별다른 일이 없으면 Idle 상태와 **Patrol** 상태를 오간다. 즉, 일정 시간이 경과하면 자동으로 Patrol 상태로 전이하고, 순찰 지점에 도착하면 다시 Idle 상태로 돌아가 일정 시간 동안 휴식을 취하는 방식이다.

플레이어가 일정 반경 이내로 적에 접근하면, 적은 내부적으로 Tick 함수를 활성화하여 매 프레임마다 플레이어가 보이는지 확인한다. 플레이어가 보이는 순간부터 적은 플레이어와 떨어진 거리에 따라 추격(**Chase**) 또는 공격(**Attack**)을 한다. 그러나, 플레이어가 충분히 멀리 도망치면 다시 Idle 상태로 진입하며, Tick 함수도 비활성화된다. 추가로, 적이 플레이어에게 피해를 입으면 원거리에서도 추격을 시작할 수 있다.

체력이 바닥나면 **Dead** 상태가 되고, 일정 시간이 경과하면 **Sink** 상태로 전이한다. 시체가 충분히 아래로 내려가 더 이상 보이지 않게 되면, 적 액터는 월드에서 제거된다.
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

#### 적 순찰 지점(PatrolPoints) 등록
<div align="center">
  <img src="http://via.placeholder.com/640x480" width="720" />

  *[적 순찰 지점 등록]*
</div>

적은 추격이나 공격 중이 아닌 동안 Idle 상태와 Patrol 상태를 왔다갔다한다. Patrol 상태에서는 개발 단계에서 등록된 순찰 지점들을 차례로 이동하며, 순찰 지점에 도착할 때마다 일정 시간 동안 Idle 상태에 머무른다. 이 순찰 지점들을 간편하게 등록하기 위해, 언리얼 엔진의 _MakeEditWidget_ 기능을 활용하였다. 게임 시작 시, 적의 생성 위치를 기준으로 상대 좌표로 등록되어 있던 순찰 지점들을 모두 월드 기준의 절대 좌표로 변환한다. 또한, 적이 처음 생성된 위치를 순찰 지점으로 추가 등록하기 때문에 미리 등록해둔 순찰 지점이 N개라면, 모두 (N + 1)개의 순찰 지점을 차례로 방문하게 된다.

```C++
/// Enemy.h 중 일부 /////

private:
  /** 순찰 지점 리스트. */
  UPROPERTY(EditAnywhere, Category = "Config", meta = (MakeEditWidget = "true"))
  TArray<FVector> PatrolPoints;

```
```C++
/// Enemy.cpp 중 일부 /////

// 시작 위치를 마지막 순찰 지점으로 추가 후 모든 순찰 지점을 월드 좌표로 변환
void AEnemy::ConvertPatrolPointsToWorld() {
  PatrolPoints.Add(FVector::ZeroVector);  // 시작 위치 추가
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

### 무기 체계 (AWeapon)
<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229324860-14f4e5b0-4e1f-4849-a9db-8381f9f14baa.png" width="720" />

  _[무기 상속 관계도]_
</div>

무기는 **Weapon** 클래스를 상속하여 구현되며, 각각의 무기는 Weapon 클래스의 공격 기능을 오버라이드하여 무기마다 고유한 특화된 공격 기능을 구현할 수 있다.

하지만, _플레이어가 무기를 장착하고 있더라도 항상 공격을 수행할 수 있는 것은 아니다_. 이미 공격 중인 상황이거나, 다른 종류의 무기로 교체하고 있거나, 칼을 휘두르고 있는 등의 상황이 있을 수 있다. 총기의 경우 총알이 없는 경우도 해당된다. 따라서, 플레이어가 왼쪽 마우스를 클릭하면 항상 공격이 수행되는 것은 아니며, 공격이 가능한 상황인지 판단하여 조건이 만족되는 경우에만 공격이 수행되도록 로직이 구성되어야 한다.

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
플레이어가 무기를 장착한 상태에서 왼쪽 마우스를 클릭하면, 장착한 무기의 **TryAttack** 메서드가 호출된다. 이때, **CanAttack** 메서드가 *true*를 반환하는 상황에서만 실제 공격이 수행되며(**Attack**), *false*를 반환하는 상황에서는 **OnAttackFailure**가 호출된다. 일반적으로, 아직 공격이 끝나지 않은 상황에서 다시 공격할 수는 없으므로, CanAttack은 공격 중이지 않을 때 true를 반환하도록 구현했다.

또한, Attack 메서드 실행 시 _**bAttacking**_ 변수를 true로 변경한 후 공격이 끝나면 다시 false로 되돌려, CanAttack이 실제 공격 중인지 여부를 반영할 수 있도록 했다. 베이스 클래스에서는 공격 시도가 실패할 때 따로 수행할 기능이 없으므로 OnAttackFailure 메서드의 구현이 비어있다. 하지만 CanAttack, Attack, OnAttackFailure 메서드는 모두 virtual로 선언되어 있으므로, 서브 클래스에서 기능의 확장이 필요하다면 이를 오버라이드할 수 있다. 예를 들어, <u>_Firearm 클래스에서 CanAttack 메서드는 공격 중일 때 뿐만 아니라, 총알이 없는 경우에도 false를 반환하며, OnAttackFailure 메서드에서 빈 탄창 사운드를 재생하도록 오버라이드 되어있다_</u>. 따라서, 총알이 없는 경우에 총기를 사용한 공격을 시도하면, 빈 탄창 사운드만 재생되고 총알은 발사되지 않는다.
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 나이프 (AKnife)
나이프를 사용한 공격 시 단순히 칼날에 부착된 콜리전을 일시적으로 활성화 한다.
```C++
/// Knife.cpp 중 일부 /////

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
**총**과 **런처**는 모두 _발사할 대상_(총알 혹은 폭약)이 필요하다. 따라서, 장전(_LoadAmmos_)과 같은 기능을 공유하고 있으며, 장전되어 있지 않다면 공격할 수 없다. 이러한 기능들은 **AFirearm** 클래스에 정의되어 있고, 총과 런처는 이를 상속하여 사용한다. 두 클래스는 공격하는 방식에 약간의 차이가 있으며, 이는 각 클래스에서 오버라이딩을 통해 구현되었다. 더 자세한 내용은 각 클래스([AGun](#총-agun) 또는 [ALauncher](#런처-alauncher))에서 확인할 수 있다.
```C++
/// Firearm.cpp 중 일부 /////

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
총이 런처와 가장 다른 점은 공격 시 실제 총알이 발사되는 것이 아니라, 라인 트레이스(**line trace**)를 통해 힛(**hit**)을 검사한다는 것이다. 검사 결과, 힛 액터가 적인 경우에 데미지를 가하고, 그 외 지점에 맞은 경우에는 힛 이펙트만 발생시킨다(ex. 돌이 튀기는 효과).
```C++
/// Gun.cpp 중 일부 /////

void AGun::Attack() {
  Super::Attack();
  EjectAmmo();  // 탄피 배출

  // 라인 트레이스 수행
  if (FHitResult HitResult; LineTrace(HitResult)) {
    if (AEnemy *HitEnemy = Cast<AEnemy>(HitResult.GetActor())) {
      // (1) 적이 맞았으면 이펙트 + 데미지
      SpawnHitFX(HitEnemy->GetDamagedFX(), HitResult);
      ApplyDamage(HitEnemy);
    } else { // (2) 다른 곳에 맞았으면 이펙트만
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
총이 라인 트레이스를 통해 공격을 수행한다면, 런처는 실제 폭약([AExplosive](#폭약-aexplosive))을 발사하고 폭약의 뇌관이 다른 액터와 오버랩하여 콜리전이 발생하면, 그 즉시 폭발하여 일정 반경 내의 액터들에 데미지를 입히는 방식으로 동작한다. 런처의 역할은 단순히 총구 위치에 폭약을 생성하는 것이고, 이를 발사시키는 것은 액터에 부착된 가속 컴포넌트([UAccelMovementComponent](#가속-uaccelmovementcomponent))의 역할이다.
```C++
/// Launcher.cpp 중 일부 /////

void ALauncher::Attack() {
  Super::Attack();

  // 카메라 방향으로 폭약의 머리가 오도록 소켓 위치에 폭약 생성
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

### 주요 컴포넌트의 핵심 기능
#### 왕복 병진운동 (UPeriodicalMovementComponent)
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

두 개의 끝점(**P**<sub>**1**</sub>과 **P**<sub>**2**</sub>) 사이에 위치한 점(**Q**)의 좌표는 <u>*내분점 공식*</u>을 이용하여 구한다. 내분 계수 **𝛼**의 값을 시간에 따라 적절히 변경하면, 내분점이 두 점 사이를 왕복하도록 할 수 있다.  
<br>

<div align="center">
  <img src="https://user-images.githubusercontent.com/84118586/229275229-f87eb1ca-30f2-476d-ba70-13d9d55edd16.png" width="360" />

  _[𝛼 = 𝛼(t) 함수의 개형]_
</div>

위 그래프는 내분 계수 𝛼의 시간에 따른 변화를 나타낸 것이다. 위와 같은 형태로 내분 계수를 변화시키면, 액터가 양 끝점에서 잠시 멈추었다가 왕복하기를 반복하는 것이 가능하다. 그래프의 개형을 결정짓는 두 매개변수(**t**<sub>**1**</sub>, **t**<sub>**2**</sub>)는 *UPROPERTY*로 선언하여 개발 과정에서 변경할 수 있도록 하였다.  

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

#### 가속 (UAccelMovementComponent)
일정한 가속도로 가속하여 최대 속도에 도달해야 하는 액터(ex. 런처에서 발사된 폭약)를 구현할 수 있다. 가속도와 최대 속도는 *UPROPERTY*로 선언되어 개발 단계에서 조정할 수 있다. 최대 속도를 넘어서 계속 가속하게 되면, 액터의 속도가 비정상적으로 커지므로 가속을 중지해야 한다. 이 컴포넌트를 사용하려면, 루트 컴포넌트가 **Simulate Physics**가 설정된 **UPrimitiveComponent**이어야 하며, 이를 BeginPlay 함수 내에서 강제하고 있다.

```C++
/// AccelMovementComponent.cpp BeginPlay 중 일부 /////

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
```
```C++
/// AccelMovementComponent.cpp 중 일부 /////

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
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 걷기-달리기 전환 (ULocomotionComponent)
플레이어가 달릴 때에는 스태미나가 추가로 감소하며, 일정량 이상의 스태미나가 있어야만 달릴 수 있다. 스태미나가 일정 수준 이하로 떨어지면 더 이상 달릴 수 없고, 걷기로 자동 전환된다.
```C++
/// LocomotionComponent.cpp 중 일부 /////

void ULocomotionComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  if (IsRunning()) {
    Stamina->Decrease(RunDecreaseRate * DeltaTime);  // 추가 감소
  }
  if (Stamina->GetValue() < MinRunStamina) {
    Walk();  // 걷기로 자동 전환
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

#### 무기 장착 (UInventoryComponent)
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

### 그 외
#### 폭약 (AExplosive)
폭약은 **부비트랩**(booby trap)과 플레이어가 사용 가능한 **폭탄**(grenade, rocket) 등 두 종류의 액터를 구현하는 데에 사용된다. 특히 후자는, 런처([ALauncher](#런처-alauncher))에 의해 생성되고 가속 컴포넌트([UAccelMovementComponent](#가속-uaccelmovementcomponent))의 도움을 받아 발사된다. 폭약은 다른 액터와 충돌 시에 폭발하며, 일정 반경 내에 있는 액터(*플레이어와 적 모두*)들에게 데미지를 입힌다.  

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

#### 포털 (APortal)
플레이어가 포털과 오버랩 하는 경우, (1)**다음 레벨로 이동** 하거나, (2)**게임이 종료**(승리)된다. _UPROPERTY_ 로 변수를 선언하여 둘 중 어느 역할을 수행할지 설정 가능하다. 논리적으로 모든 레벨은 하나 이상의 포털을 가지고 있어야 한다. 따라서, 게임 시작 시 레벨에 포털이 있는지 확인하고, 없다면 이를 개발자에게 알리도록 하여 혹시 모를 버그를 미연에 방지하였다.

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

#### 플레이어 오버랩 (AShooterOverlap)
_아이템_, [_포털_](#포털-aportal), 그리고 _세이브 포인트_ 는 모두 플레이어와 오버랩될 때 특정 기능을 수행하며, 몬스터(적)와 같은 다른 액터들과는 충돌하지 않아야 한다. 이를 위해서, 이 세 클래스는 **AShooterOverlap**이라는 베이스 클래스를 공유하며, 베이스 클래스에서는 오버랩 시 상호작용하는 액터를 확인한다. 따라서, 서브 클래스의 **OnShooterOverlap** 메서드에서는 별다른 확인 없이 플레이어와 오버랩되었음을 보장받을 수 있고, 이 메서드를 오버라이드하여 오버랩 시 동작을 클래스마다 다르게 정의할 수 있다.

```C++
/// ShooterOverlap.h 중 일부 /////

protected:
  /** 플레이어 오버랩 시 호출. */
  virtual void OnShooterOverlap(class AShooter *Shooter);
  
private:
  /** 폰 오버랩 이벤트 델리게이트. */
  UFUNCTION()
  void OnSphereBeginOverlap(
      UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, 
      UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
      bool bFromSweep, const FHitResult &SweepResult);
```
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

// Shooter가 플레이어임이 보장됨
void AShooterOverlap::OnShooterOverlap(class AShooter *Shooter) {
  if (bPlayEffectsOnOverlap) PlayOverlapEffects();
}

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
아래는 아이템, 포털, 그리고 세이브 포인트 각 클래스마다 다르게 오버라이드된 **OnShooterOverlap** 메서드의 구현이다.
```C++
/// Item.cpp 중 일부 /////

// 아이템의 종류로 총알, 무기, 포션이 있음
// (1) 총알 아이템은 즉시 획득하여 인벤토리에 추가
// (2) 무기 아이템은 같은 종류의 무기를 장착 중인지 먼저 확인 후 획득
// (3) 포션 아이템은 즉시 획득하여 복용
void AItem::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  switch (ItemType) {
    case EItemType::EIT_Ammo:   EquipAmmo(Shooter);       break;  // (1)
    case EItemType::EIT_Weapon: TryEquipWeapon(Shooter);  break;  // (2)
    case EItemType::EIT_Potion: ApplyPotion(Shooter);     break;  // (3)
  }
}
```
```C++
/// Portal.cpp 중 일부 /////

void APortal::OnShooterOverlap(class AShooter *Shooter) {
  Super::OnShooterOverlap(Shooter);
  if (bEndOfGame) {
    GameMode->ClearGame();  // 게임 클리어 (승리)
  } else {
    GameMode->ClearLevel(NextLevel);  // 다음 레벨로 이동
  }
}
```
```C++
/// SavePoint.cpp 중 일부 /////

void ASavePoint::OnShooterOverlap(class AShooter *Shooter) {
  if (bActivated) {
    Super::OnShooterOverlap(Shooter);
    GameMode->SaveGame();  // 현재까지 진행 상황 저장
  }
  Destroy();
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

#### 체력 회복 (Stamina)
플레이어가 달리고 있지 않으면 스태미나는 일정한 속도로 감소하며, 체력은 스태미나의 양에 비례하여 회복된다. 스태미나가 100%일 때는 가장 빠른 속도로 회복되며, 스태미나가 바닥인 상태에서는 회복이 전혀 이루어지지 않는다.
```C++
/// Shooter.cpp 중 일부 /////

// 스태미나가 0이 아니라면 틱 함수는 동작 중에 있음
void AShooter::Tick (float DeltaSeconds) {
  Super::Tick(DeltaSeconds);

  InterpSpringArmOffset(DeltaSeconds);
  Stamina->Decrease(NormalDecreaseRate * DeltaSeconds);  // 스태미나 감소
  RecoverHealth(MaxRecoveryRate * Stamina->GetRatio() * DeltaSeconds);  // 체력 회복

  if (ShouldDisableTick()) SetActorTickEnabled(false);
}
```
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>

---
## 구현 예정
- 총기 발사 시 카메라 진동
- 저격 소총 줌 기능
- 적 스킬 사용
- 무기 교체 도중 공격 금지
<!-- 위로가기 아이콘 ----->
<div align="right">

  [&#128285;](#목차)
</div>
<br>
