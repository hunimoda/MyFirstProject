#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.h"
#include "../../Weapon/WeaponType.h"
#include "HeadsUpDisplay.generated.h"

UCLASS()
class MYFIRSTPROJECT_API UHeadsUpDisplay : public UUserWidget {
	GENERATED_BODY()

private:
	/** 플레이어의 남은 목숨 수 표시. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *PlayerLives;
	/** 게임 플레이 시간(분) 표시. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *MinuteText;
	/** 게임 플레이 시간(초) 표시. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *SecondText;
	/** 레벨에 있는 에너미 수 표시. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *EnemyCount;
	/** LOG 매크로의 메시지 출력. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *LogMessage;
	/** 크로스헤어. */
	UPROPERTY(meta = (BindWidget))
	class UImage *Crosshair;
	/** 무기 정보 표시. */
	UPROPERTY(meta = (BindWidget))
	class UBorder *WeaponStats;
	/** 화기의 사격 모드(auto). */
	UPROPERTY(meta = (BindWidget))
	class UBorder *AutoMode;
	/** 무기 이미지. */
	UPROPERTY(meta = (BindWidget))
	class UImage *WeaponImage;
	/** 장전된 총알의 수. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *LoadedAmmosText;
	/** 아직 장전되지 않은 총알의 수. */
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *UnloadedAmmosText;
	/** 플레이어의 스태미나 바. */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *StaminaBar;
	/** 플레이어의 체력 바. */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *HealthBar;
	/** 미니맵. */
	UPROPERTY(meta = (BindWidget))
	class UImage *Minimap;

private:
	/** 무기별 크로스헤어. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TMap<EWeaponType, FCrosshair> Crosshairs;
	/** 무기별 이미지. */
	UPROPERTY(EditAnywhere, Category = "Config")
	TMap<EWeaponType, class UTexture2D *> WeaponTextures;
	/** 로그 메시지 출력 시간. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config",
						meta = (AllowPrivateAccess = "true"))
	float LogDisplayTime;
	/** 스태미나의 양이 충분할 때 스태미나 바 색상. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Stamina Bar",
						meta = (AllowPrivateAccess = "true"))
	FLinearColor StaminaBarNormalColor;
	/** 스태미나의 양이 부족할 때 스태미나 바 색상. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config|Stamina Bar",
						meta = (AllowPrivateAccess = "true"))
	FLinearColor StaminaBarShortageColor;
	/** 미니맵 최소화 크기(픽셀). */
	UPROPERTY(EditAnywhere, Category = "Config|Minimap")
	float MinimapMinSize;
	/** 미니맵 최대화 크기(픽셀). */
	UPROPERTY(EditAnywhere, Category = "Config|Minimap")
	float MinimapMaxSize;

public:
	/** 생성자. */
	UHeadsUpDisplay(const FObjectInitializer &ObjectInitializer);

protected:
	/** HUD가 뷰포트에 추가될 때 호출. */
	virtual void NativeConstruct() override;

public:
	/** 플레이어 목숨 수 업데이트. */
	void UpdatePlayerLives();
	/** 게임 플레이 시간 업데이트. */
	void UpdatePlayTime(uint32 Minute, uint32 Second);
	/** 에너미 수 업데이트. */
	void UpdateEnemyCount();
	/** 일정 시간 동안 로그 메시지 출력. */
	void DisplayLogMessage(const FString &Message);
	/** 현재 사용 중인 무기 정보로 업데이트. */
	void UpdateWeapon();
	/** 연사 표시(A)를 보임|숨김. */
	void DisplayAuto(bool bDisplay);
	/** 장전된 총알의 수를 빈칸으로 둠. */
	void UpdateLoadedAmmos();
	/** 장전된 총알의 수를 업데이트. */
	void UpdateLoadedAmmos(uint32 LoadedAmmos);
	/** 장전되지 않은 총알의 수 업데이트. */
	void UpdateUnloadedAmmos(uint32 UnloadedAmmos);
	/** 플레이어 스태미나 업데이트. */
	void UpdateStamina(float Stamina);
	/** 플레이어 체력 업데이트. */
	void UpdateHealth(float Health);
	/** 미니맵 사이즈 변경. */
	void ToggleMinimapSize();

private:
	/** 로그 메시지를 숨김. */
	void HideLogMessage();
	/** 무기 관련 위젯을 보임|숨김. */
	void DisplayWeaponStats(bool bDisplay);
	/** 크로스헤어 변경. */
	void UpdateCrosshair(EWeaponType Type);
	/** 무기 이미지 변경. */
	void UpdateWeaponImage(EWeaponType Type);

private:
	/** 로그 메시지 출력 후 일정 시간 경과 후에 숨기도록 하는 타이머. */
	FTimerHandle HideLogTimer;
	/** 게임 모드. */
	class AMyGameMode *GameMode;
	/** 플레이어. */
	class AShooter *Shooter;
	/** 미니맵 사이즈가 최대. */
	bool bLargeMinimap;
};