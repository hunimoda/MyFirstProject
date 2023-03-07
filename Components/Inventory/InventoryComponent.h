#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Weapon/WeaponType.h"
#include "InventoryComponent.generated.h"

/** 전달 인자로 EWeaponType을 받는 델리게이트 선언. */
DECLARE_DELEGATE_OneParam(FEWeaponTypeDelegate, EWeaponType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYFIRSTPROJECT_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	/** 현재 오버랩 중인 무기 아이템. 없으면 nullptr. */
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	class AItem *OverlappingItem;

private:
	/** 보유 중인 무기 리스트. 키에 해당하는 무기 타입이 없으면 값으로 nullptr 저장. */
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	TMap<EWeaponType, class AWeapon *> Weapons;
	/** 보유 중인 총알의 수. Launcher 타입 무기의 경우 Grenade의 수. */
	UPROPERTY(EditAnywhere, Category = "Stats")
	TMap<TSubclassOf<class AFirearm>, uint32> UnloadedAmmos;
	/** 현재 사용 중인 무기. 빈손이면 nullptr. */
	UPROPERTY(VisibleAnywhere, Category = "Stats|Active Weapon")
	class AWeapon *ActiveWeapon;
	/** 직전에 사용한 무기. 현재 빈손인 경우에만 유효. */
	UPROPERTY(VisibleAnywhere, Category = "Stats|Active Weapon")
	class AWeapon *WeaponToActivate;
	
public:	
	/** 디폴트 생성자. */
	UInventoryComponent();
	/** 게임 시작 전 호출. */
	virtual void InitializeComponent() override;
	/** 액션 및 축 매핑. */
	void SetupInputComponent(class UInputComponent *InputComponent);

public:
	/** 해당 타입의 무기를 보유 중이면 사용. */
	void UseWeaponOfType(EWeaponType Type);
	/** 오버랩 중인 아이템으로 무기 교체. */
	void EquipOverlappingWeapon();
	/** 무장 중이면 무장 해제, 빈손이면 직전 무기로 무장. */
	void ToggleWeapon();
	/** 연사|단발 전환 시도. */
	void TryToggleAuto();
	/** 장전. */
	void Reload();
	/** 무기 장착 시도. */
	void TryEquipWeapon(class AItem *WeaponItem);
	/** 해당 타입의 총알 수 증가. */
	void EquipAmmo(TSubclassOf<class AFirearm> TargetWeapon, uint32 AmmoCount);
	/** 총알 수를 최대 MaxDecrease만큼 줄임. */
	uint32 DecreaseAmmos(TSubclassOf<class AFirearm> TargetWeapon,
											 uint32 MaxDecrease);
	/** 인벤토리 내 무기 정보 저장. */
	void Save(struct FInventoryStats &InventoryStats);
	/** 저장된 무기 정보 로드. */
	void Load(const struct FInventoryStats &InventoryStats);
	/** 해당 화기에 쓰는 총알의 수 반환. */
	uint32 GetUnloadedAmmos(TSubclassOf<class AFirearm> TargetWeapon) const;
	/** 현재 사용 중인 무기 반환. */
	FORCEINLINE class AWeapon *GetActiveWeapon() const { return ActiveWeapon; }

private:
	/** 무기 클래스로부터 무기의 타입과 이름 추출. */
	TPair<EWeaponType, FString> ExtractTypeAndName(
			TSubclassOf<class AWeapon> WeaponClass) const;
	/** 무기 사용. */
	void UseWeapon(class AWeapon *NewWeapon);
	/** 무기 블루프린트로부터 무기 장착. */
	class AWeapon *EquipWeapon(TSubclassOf<class AWeapon> NewWeaponClass);
	/** 무기 아이템으로부터 무기 장착. */
	class AWeapon *EquipWeapon(class AItem *Item);
	/** 전달받은 무기 블루프린트 타입을 사용 중이면 HUD 업데이트. */
	void UpdateAmmosIfUsing(TSubclassOf<class AFirearm> WeaponBP);
	/** 무기 종류로부터 무기가 장착될 소켓의 이름을 문자열 형태로 반환. */
	FString GetSocketNameFromType(EWeaponType Type);
	/** 무기를 오른손에(서) 부(탈)착*/
	void AttachWeaponToHand(class AWeapon *Weapon, bool bAttach = true);

private:
	/** 플레이어. */
	class AShooter *Shooter;
};