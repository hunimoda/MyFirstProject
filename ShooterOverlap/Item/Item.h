#pragma once

#include "CoreMinimal.h"
#include "ItemType.h"
#include "../ShooterOverlap.h"
#include "Item.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AItem : public AShooterOverlap {
	GENERATED_BODY()

private:
	/** 아이템 종류. */
	UPROPERTY(EditAnywhere, Category = "Config")
	EItemType ItemType;
	/** 총알이 사용되는 무기의 타입. */
	UPROPERTY(EditAnywhere, Category = "Config|Ammo")
	TSubclassOf<class AFirearm> TargetWeapon;
	/** 획득하게 되는 총알의 수. */
	UPROPERTY(EditAnywhere, Category = "Config|Ammo")
	uint32 AmmoCount;
	/** 아이템으로 획득하게 되는 무기의 블루프린트 클래스. */
	UPROPERTY(EditAnywhere, Category = "Config|Weapon")
	TSubclassOf<class AWeapon> WeaponBP;
	/** 포션의 효과. */
	UPROPERTY(EditAnywhere, Category = "Config|Potion")
	EPotionEffect PotionEffect;
	/** 효과의 정도. */
	UPROPERTY(EditAnywhere, Category = "Config|Potion")
	float EffectAmount;

public:
	/** 디폴트 생성자. */
	AItem();
	/** 무기 블루프린트 클래스 반환. */
	FORCEINLINE TSubclassOf<class AWeapon> GetWeaponBP() const { return WeaponBP; }

protected:
	/** 플레이어 오버랩 시 호출. */
	virtual void OnShooterOverlap(class AShooter *Shooter) override;

private:
	/** 총알 획득. */
	void EquipAmmo(class AShooter *Shooter);
	/** 무기 장착 시도. */
	void TryEquipWeapon(class AShooter *Shooter);
	/** 포션 효과 적용. */
	void ApplyPotion(class AShooter *Shooter);
};