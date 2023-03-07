#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AMyGameMode : public AGameModeBase {
	GENERATED_BODY()

public:
	/** 디폴트 생성자. */
	AMyGameMode();

protected:
	/** 게임 시작 시 호출. */
	virtual void BeginPlay() override;

public:
	/** 매 프레임 호출. */
	virtual void Tick(float DeltaSeconds) override;

public:
	/** 월드에 존재하는 에너미 수 반환. */
	uint32 GetEnemyCount();
	/** 에너미가 죽으면 호출. */
	void HandleEnemyDeath(AController *EventInstigator);
	/** 플레이어가 죽으면 호출. */
	void HandlePlayerDeath();
	/** 게임 플레이 결과를 문자열 형태로 반환. */
	UFUNCTION(BlueprintCallable)
	FString GetGameplayResult() const;
	/** 플레이어 목숨 수 반환. */
	FORCEINLINE uint32 GetPlayerLives() const { return PlayerLives; }

public:
	/** 게임을 로드 하지 않고 새 게임을 시작. */
	UFUNCTION(BlueprintCallable)
	void PlayNewGame();
	/** 기존에 하던 게임을 로드. */
	UFUNCTION(BlueprintCallable)
	void LoadGame();
	/** 현재 상태로 게임 저장. */
	void SaveGame();
	/** 현재 상태로 게임 저장. */
	void SaveGame(const FString &Level, bool bPlayerStart = false);
	/** 게임을 처음부터 다시 시작. */
	UFUNCTION(BlueprintCallable)
	void RestartGame();
	/** 게임을 그만두고 프로그램을 종료. */
	UFUNCTION(BlueprintCallable)
	void QuitGame();
	/** 게임을 클리어 하고 승리. */
	void ClearGame();
	/** 레벨을 클리어 하고 다음 레벨로 이동. */
	void ClearLevel(const FString &NextLevel);

private:
	/** 리스트로부터 에너미 재생성. */
	void RecreateEnemiesFromList(
			const TArray<struct FEnemyProfile> &EnemyProfiles);
	/** 에너미 정보로부터 에너미(1EA) 생성. */
	void CreateEnemyFromProfile(const struct FEnemyProfile &EnemyProfile);
	/** 월드에 존재하는 에너미의 정보로 인자로 전달된 리스트를 채움. */
	void PopulateListWithEnemies(
			TArray<struct FEnemyProfile> &EnemyProfiles);
	/** 에너미 수 초기화. */
	void InitEnemyCount(uint32 NewEnemyCount);
	/** 포탈 초기화. */
	void InitializePortal();
	/** 게임 플레이 시간 업데이트. */
	void UpdatePlaySeconds(float DeltaSeconds);
	/** PIE 환경에서도 정적 변수인 bFirstPlay가 초기화 되게 함. */
	UFUNCTION(BlueprintCallable)
	void ResetFirstPlayPIE();

private:
	/** 플레이어 컨트롤러. */
	class AShooterController *ShooterController;
	/** 플레이어. */
	class AShooter *Shooter;
	/** 포탈. */
	class APortal *Portal;
	/** 에너미 수 초기화 여부 기록. */
	bool bInitEnemyCount;
	/** 월드에 존재하는 에너미의 수 저장. */
	uint32 EnemyCount;
	/** 플레이어 남은 목숨 수. */
	uint32 PlayerLives;
	/** 플레이어의 킬 수. */
	uint32 KillCount;
	/** 게임 플레이 시간. */
	float PlaySeconds;

private:
	/** 게임 실행 후 첫 플레이인지 여부. */
	static bool bFirstPlay;
};