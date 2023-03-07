#pragma once

#include "CoreMinimal.h"
#include "../Weapon.h"
#include "Firearm.generated.h"

UCLASS()
class MYFIRSTPROJECT_API AFirearm : public AWeapon {
	GENERATED_BODY()

public:
	/** 현재 장전된 총알 수. */
	UPROPERTY(EditInstanceOnly, Category = "Stats")
	uint32 LoadedAmmos;

protected:
	/** 탄창 크기. 또는 최대 장전할 수 있는 총알 수. */
	UPROPERTY(EditAnywhere, Category = "Config|Ammo")
	uint32 MaxLoadAmmos;
	
private:
	/** 발사 시 총구에 보이는 이펙트. */
	UPROPERTY(EditAnywhere, Category = "Config|FX")
	class UParticleSystem *MuzzleFX;
	/** 발사 시 후미에 보이는 이펙트 i.e. 로켓포 후화. */
	UPROPERTY(EditAnywhere, Category = "Config|FX")
	class UParticleSystem *MuzzleRearFX;
	/** 장전 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *ReloadSound;
	/** 빈 탄창 사운드. */
	UPROPERTY(EditAnywhere, Category = "Config|Sound")
	class USoundBase *NoAmmoSound;

public:
	/** 디폴트 생성자. */
	AFirearm();
	/** 장전. */
	void LoadAmmos();

protected:
	/** 공격 가능하면 true 반환. */
	virtual bool CanAttack() override;
	/** 공격. */
	virtual void Attack() override;
	/** 공격 실패 시 호출. */
	virtual void OnAttackFailure() override;
};