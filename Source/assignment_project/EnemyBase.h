#pragma once

#include "CoreMinimal.h"
#include "SuperSideScrollerCharacter.h"
#include "EnemyBase.generated.h"

class AAIPoints;

/**
 * 실습 12.07: 기본 적 C++ 클래스 (부모: SuperSideScrollerCharacter)
 * - 앞뒤로 정찰만 하며 공격 기능은 없고, 플레이어와 부딪혀야만 피해를 줍니다.
 * - 실습 14.07 / 14.09: DestroyEnemy() 로 파괴되며 DeathEffect / DeathSound 를 재생합니다.
 * - 실습 13.09: 레벨에 배치된 BP_AIPoints 액터를 Patrol Points 로 참조합니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API AEnemyBase : public ASuperSideScrollerCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/** 이 적이 정찰할 위치 액터 (인스턴스 편집 가능) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (DisplayName = "Patrol Points"))
	TObjectPtr<AAIPoints> PatrolPoints;

	/** 실습 14.07: 적을 파괴합니다. (실습 14.09: 파괴 시 VFX/SFX 재생) */
	void DestroyEnemy();

	/** 실습 14.09: 적이 파괴될 때 재생할 파티클 이펙트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UParticleSystem* DeathEffect;

	/** 실습 14.09: 적이 파괴될 때 재생할 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USoundBase* DeathSound;

protected:
	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
};
