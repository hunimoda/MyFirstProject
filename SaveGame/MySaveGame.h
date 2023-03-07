#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerStats.h"
#include "EnemyProfile.h"
#include "MySaveGame.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UMySaveGame : public USaveGame {
	GENERATED_BODY()

public:
	/** 레벨 처음 위치에서 게임 시작. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	bool bPlayerStart;
	/** 레벨 이름. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString Level;
	/** 게임 플레이 시간(초). */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	float PlaySeconds;
	/** 플레이어의 킬 수. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 KillCount;
	/** 플레이어의 남은 목숨 수. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 PlayerLives;
	/** 에너미 정보. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	TArray<FEnemyProfile> EnemyProfiles;
	/** 플레이어 정보. */
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FPlayerStats PlayerStats;

public:
	/** 저장된 게임 정보가 있음. */
	static bool HasSavedGame();
	/** 로드 게임 인스턴스 반환. */
	static UMySaveGame *MakeLoadGameInstance();
	/** 저장 게임 인스턴스 반환. */
	static UMySaveGame *MakeSaveGameInstance();
	/** 게임 정보 저장. */
	static void SaveGame(UMySaveGame *SaveGameInstance);
	/** 저장된 게임이 있으면 삭제. */
	static void DeleteGame();

private:
	/** 저장할 슬롯의 이름. */
	static FString SlotName;
	/** 저장할 인덱스. */
	static uint32 UserIndex;
};