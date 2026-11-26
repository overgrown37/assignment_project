#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SuperSideScroller_Brick.generated.h"

/**
 * 실습 15.07 / 15.08: 코인이 숨겨져 있는 벽돌
 * - BrickMesh(BlockAll) 가 루트이며, 아래쪽으로 살짝 튀어나온 BrickCollision(박스) 이 있습니다.
 * - 플레이어가 점프해서 아래에서 BrickCollision 을 치면, bHasCollectable 인 경우
 *   코인을 추가하고 HitSound / Explosion 을 재생한 뒤 파괴됩니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API ASuperSideScroller_Brick : public AActor
{
	GENERATED_BODY()

public:
	ASuperSideScroller_Brick();

	/** 실습 15.07: 벽돌이 파괴될 때 재생할 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Brick)
	class USoundBase* HitSound;

	/** 실습 15.07: 벽돌이 파괴될 때 재생할 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Brick)
	class UParticleSystem* Explosion;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Brick)
	class UStaticMeshComponent* BrickMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Brick)
	class UBoxComponent* BrickCollision;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	/** 이 벽돌에 코인이 들어 있는지 여부 */
	UPROPERTY(EditAnywhere, Category = Brick)
	bool bHasCollectable;

	/** 이 벽돌에서 얻을 수 있는 코인 개수 */
	UPROPERTY(EditAnywhere, Category = Brick)
	int32 CollectableValue = 1;

	/** 실습 15.08 */
	void AddCollectable(class ASuperSideScroller_Player* Player);
	void PlayHitSound();
	void PlayHitExplosion();
};
