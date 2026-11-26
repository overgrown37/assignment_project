#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerProjectile.generated.h"

/**
 * 실습 13.11 / 13.12: 플레이어 발사체
 * - 중력을 사용하지 않고 800 의 일정한 속도로 날아가며 3초 후 소멸
 * - 어떤 표면에 부딪혀도 파괴되며, 적에게 맞으면 적을 한 번에 파괴합니다.
 * - 실습 14.08 / 활동 14.01: 적과 충돌하면 Enemy->DestroyEnemy(), 이후 ExplodeProjectile()
 * - 실습 14.10: 날아가는 동안 재생되는 이펙트(ProjectileEffect) 와 사운드(ProjectileMovementSound)
 * - 활동 14.02: 파괴될 때 DestroyEffect / DestroySound 재생
 */
UCLASS()
class ASSIGNMENT_PROJECT_API APlayerProjectile : public AActor
{
	GENERATED_BODY()

public:
	APlayerProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	//스피어 콜리전 컴포넌트
	//발사체가 적 및 환경 에셋과의 충돌을 인식하는 데 사용할 수 있습니다.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** 실습 14.08: 발사체 폭발(파괴). 활동 14.02: 파괴 시 VFX / SFX 재생 */
	void ExplodeProjectile();

	/** 활동 14.02: 발사체가 파괴될 때 재생할 파티클 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	class UParticleSystem* DestroyEffect;

	/** 활동 14.02: 발사체가 파괴될 때 재생할 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	class USoundBase* DestroySound;

protected:
	virtual void BeginPlay() override;

private:
	//프로젝타일 무브먼트 컴포넌트
	//발사체의 움직임을 위해 사용합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	//스태틱 메시 컴포넌트
	//이 발사체를 시각적으로 표현하여 게임 내에서 볼 수 있도록 합니다.
	UPROPERTY(EditAnywhere, Category = Projectile)
	class UStaticMeshComponent* MeshComp;

	/** 실습 14.10: 발사체가 날아가는 동안 재생되는 사운드 */
	UPROPERTY(VisibleDefaultsOnly, Category = Sound)
	class UAudioComponent* ProjectileMovementSound;

	/** 실습 14.10: 발사체에 붙어 함께 이동하는 파티클 이펙트 */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UParticleSystemComponent* ProjectileEffect;

	bool bExploded = false;
};
