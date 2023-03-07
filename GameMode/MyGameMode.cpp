#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameMapsSettings.h"

#include "../Controller/ShooterController.h"
#include "../Characters/Enemy/Enemy.h"
#include "../Characters/Shooter/Shooter.h"
#include "../HUD/HeadsUpDisplay/HeadsUpDisplay.h"
#include "../SaveGame/MySaveGame.h"
#include "../ShooterOverlap/Portal/Portal.h"

// 정적 멤버 초기화
bool AMyGameMode::bFirstPlay = true;

AMyGameMode::AMyGameMode() {
  PrimaryActorTick.bCanEverTick = true;
  
  // 디폴트 폰|컨트롤러 설정
  ConstructorHelpers::FClassFinder<APawn> DefaultPawnAsset(
      TEXT("Blueprint'/Game/MyFirstProject/Character/BP_Shooter'"));
  DefaultPawnClass = DefaultPawnAsset.Class;
  ConstructorHelpers::FClassFinder<AShooterController> DefaultControllerAsset(
      TEXT("Blueprint'/Game/MyFirstProject/BP_ShooterController.BP_ShooterController_C'"));
  PlayerControllerClass = DefaultControllerAsset.Class;

  // 멤버 변수 초기화
  bInitEnemyCount = false;
  EnemyCount = 0;
  PlayerLives = 3;
  KillCount = 0;
  PlaySeconds = 0.f;
}

void AMyGameMode::BeginPlay() {
  Super::BeginPlay();

  // 멤버 변수 초기화
  ShooterController = Cast<AShooterController>(
      UGameplayStatics::GetPlayerController(this, 0));
  Shooter = Cast<AShooter>(ShooterController->GetPawn());
  InitializePortal();

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
}

void AMyGameMode::Tick(float DeltaSeconds) {
  Super::Tick(DeltaSeconds);
  UpdatePlaySeconds(DeltaSeconds);
}

// 초기화 되지 않았으면 월드에 존재하는 에너미 수로 초기화 후 반환
uint32 AMyGameMode::GetEnemyCount() {
  if (!bInitEnemyCount) {
    TArray<AActor *> Enemies;
    UGameplayStatics::GetAllActorsOfClass(this, AEnemy::StaticClass(), Enemies);
    InitEnemyCount(Enemies.Num());
  }
  return EnemyCount;
}

// (1) 에너미를 죽인 것이 플레이어인 경우에만 킬 수 증가
// (2) 에너미를 모두 처치했으면 포탈을 작동시킴
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

// 킬 수가 0일 때와 아닐 때로 구분하여 게임 플레이 결과를 보임
FString AMyGameMode::GetGameplayResult() const {
  if (!KillCount) return TEXT("그래도 0킬은 너무하잖아?");
  uint32 Minute = (uint32)PlaySeconds / 60;
  uint32 Second = (uint32)PlaySeconds % 60;
  return FString::Printf(TEXT("%d분 %d초 동안 %d킬 달성!"),
                         Minute, Second, KillCount);
}

void AMyGameMode::PlayNewGame() {
  bFirstPlay = false;
  UMySaveGame::DeleteGame();
  ShooterController->StartHUD();
}

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

// 세이브 포인트에 도달한 경우 게임을 저장하는 방법
// 게임 로드 시 현재 레벨에서 세이브 포인트가 있던 위치에서 플레이어 스폰
void AMyGameMode::SaveGame() {
  SaveGame(UGameplayStatics::GetCurrentLevelName(this));
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

void AMyGameMode::RestartGame() {
  UGameplayStatics::OpenLevel(
      this, FName(*UGameMapsSettings::GetGameDefaultMap()));
}

void AMyGameMode::QuitGame() {
  UKismetSystemLibrary::QuitGame(
      this, ShooterController, EQuitPreference::Quit, false);
}

// 마지막 레벨까지 클리어 했을 때 호출되는 점에서 ClearLevel과 다름
void AMyGameMode::ClearGame() {
  UMySaveGame::DeleteGame();
  ShooterController->DisplayWinScreen();
}

// 레벨 클리어 시 자동으로 게임 저장 (마지막 레벨 제외)
// 다음 레벨로 게임을 저장하고 플레이어 스폰 위치는 PlayerStart
void AMyGameMode::ClearLevel(const FString &NextLevel) {
  SaveGame(NextLevel, true);
  UGameplayStatics::OpenLevel(this, FName(*NextLevel));
}

void AMyGameMode::RecreateEnemiesFromList(
    const TArray<struct FEnemyProfile> &EnemyProfiles) {
  // 월드에 존재하던 에너미를 모두 삭제
  TArray<AActor *> Enemies;
  UGameplayStatics::GetAllActorsOfClass(this, AEnemy::StaticClass(), Enemies);
  for (AActor *Enemy : Enemies) Enemy->Destroy();

  // 전달된 리스트 바탕으로 에너미 다시 생성
  for (const FEnemyProfile &EnemyProfile : EnemyProfiles) {
    CreateEnemyFromProfile(EnemyProfile);
  }
  InitEnemyCount(EnemyProfiles.Num());
}

// (*) 에너미 위치가 정해지는 것은 AEnemy::Load 내에서이므로 콜리전에 관계없이 무조건 스폰
void AMyGameMode::CreateEnemyFromProfile(
    const struct FEnemyProfile &EnemyProfile) {
  FActorSpawnParameters SpawnParameters;
  SpawnParameters.SpawnCollisionHandlingOverride =
      ESpawnActorCollisionHandlingMethod::AlwaysSpawn;  // (*)
  AEnemy *SpawnedEnemy =
      GetWorld()->SpawnActor<AEnemy>(EnemyProfile.Class, SpawnParameters);
  SpawnedEnemy->Load(EnemyProfile.Stats);
}

void AMyGameMode::PopulateListWithEnemies(
    TArray<struct FEnemyProfile> &EnemyProfiles) {
  // 월드에 존재하는 에너미 수집
  TArray<AActor *> EnemyActors;
  TSubclassOf<AActor> EnemyClass = AEnemy::StaticClass();
  UGameplayStatics::GetAllActorsOfClass(this, EnemyClass, EnemyActors);

  // 리스트에 에너미 정보 추가
  for (AActor *EnemyActor : EnemyActors) {
    AEnemy *Enemy = Cast<AEnemy>(EnemyActor);
    if (!Enemy->IsDead()) {
      FEnemyProfile EnemyProfile;
      EnemyProfile.Class = Enemy->GetClass();
      Enemy->Save(EnemyProfile.Stats);
      EnemyProfiles.Add(EnemyProfile);
    }
  }
}

// 에너미를 초기화 하고, 레벨에 남은 에너미가 있으면 포탈을 숨김
void AMyGameMode::InitEnemyCount(uint32 NewEnemyCount) {
  EnemyCount = NewEnemyCount;
  bInitEnemyCount = true; 
  Portal->SetActivated(!EnemyCount);
}

// 레벨마다 포탈이 반드시 존재하도록 강제함
void AMyGameMode::InitializePortal() {
  TSubclassOf<AActor> PortalClass = APortal::StaticClass();
  Portal = Cast<APortal>(UGameplayStatics::GetActorOfClass(this, PortalClass));
  if (!Portal) {
    UE_LOG(LogTemp, Fatal, TEXT("현재 레벨에 포탈이 존재하지 않습니다."));
  }
}

// 매 프레임 호출되는 함수이므로 HUD의 업데이트는 초가 바뀔 때에만 하도록 함
void AMyGameMode::UpdatePlaySeconds(float DeltaSeconds) {
  uint32 OldPlaySecondsInt = (uint32)PlaySeconds;
  uint32 NewPlaySecondsInt = (uint32)(PlaySeconds + DeltaSeconds);
  PlaySeconds += DeltaSeconds;  // 게임 플레이 시간 업데이트

  // HUD에 게임 플레이 시간 업데이트
  if (NewPlaySecondsInt != OldPlaySecondsInt) {
    uint32 Minute = NewPlaySecondsInt / 60;
    uint32 Second = NewPlaySecondsInt % 60;
    if (HUD) HUD->UpdatePlayTime(Minute, Second);
  }
}

void AMyGameMode::ResetFirstPlayPIE() { bFirstPlay = true; }