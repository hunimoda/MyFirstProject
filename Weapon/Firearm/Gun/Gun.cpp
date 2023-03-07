#include "Gun.h"
#include "Kismet/GameplayStatics.h"

#include "Ammo/Ammo.h"
#include "../../../Characters/Enemy/Enemy.h"
#include "../../../Characters/Shooter/Shooter.h"

// 멤버 변수 초기화
AGun::AGun() {
	TraceLength = 0.f;
	AmmoClass = nullptr;
}

void AGun::Attack() {
  Super::Attack();
	EjectAmmo();  // 탄피 배출

	// 라인 트레이스 수행
	if (FHitResult HitResult; LineTrace(HitResult)) {
		if (AEnemy *HitEnemy = Cast<AEnemy>(HitResult.GetActor())) {
			// (1) 에너미가 맞은 경우
			SpawnHitFX(HitEnemy->GetDamagedFX(), HitResult);
			ApplyDamage(HitEnemy);
		} else { // (2) 다른 곳에 맞은 경우
			SpawnHitFX(HitFX, HitResult);
		}
	}
}

bool AGun::LineTrace(FHitResult &OutHit) {
	// 라인 트레이스를 위한 파라미터 준비
	FVector Start = Mesh->GetSocketLocation(TEXT("MuzzleFlash"));
	FVector Direction = Shooter->GetControlRotation().Vector();
	FVector End = Start + TraceLength * Direction;

	// 라인 트레이스 수행 후 결과 반환
	return GetWorld()->LineTraceSingleByChannel(
			OutHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
}

// 맞은 지점의 법선 방향으로 이펙트를 보임
void AGun::SpawnHitFX(
    class UParticleSystem *FX, const FHitResult &HitResult) {
	FRotator FXRotation =
			FRotationMatrix::MakeFromZ(HitResult.ImpactNormal).Rotator();
	UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), FX, HitResult.ImpactPoint, FXRotation);
}

// 탄피 생성 위치는 소켓의 위치와 동일하나, 총기와 나란해야 하므로 회전은 총기의 회전을 사용
void AGun::EjectAmmo() {
	FVector SocketLocation = Mesh->GetSocketLocation(TEXT("AmmoEject"));
	GetWorld()->SpawnActor<AAmmo>(AmmoClass, SocketLocation, GetActorRotation());
}