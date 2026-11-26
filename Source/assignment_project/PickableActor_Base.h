#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableActor_Base.generated.h"

/**
 * 실습 15.01 / 활동 15.01: 수집용 코인과 파워업 물약의 베이스 클래스
 * - CollisionComp(구체, OverlapAllDynamic) 가 루트, MeshComp 는 콜리전 없음
 * - RotationComp(URotatingMovementComponent) 로 계속 회전
 * - 플레이어와 겹치면 PlayerPickedUp() -> PickupSound 재생 후 파괴
 */
UCLASS()
class ASSIGNMENT_PROJECT_API APickableActor_Base : public AActor
{
	GENERATED_BODY()

public:
	APickableActor_Base();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = PickableItem)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = PickableItem)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = PickableItem)
	class URotatingMovementComponent* RotationComp;

	/** 활동 15.01: 플레이어가 아이템을 주웠을 때 호출 */
	UFUNCTION(BlueprintCallable, Category = PickableItem)
	virtual void PlayerPickedUp(class ASuperSideScroller_Player* Player);

	/** 활동 15.01: CollisionComp 의 OnComponentBeginOverlap 콜백 */
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 활동 15.01: 아이템을 주웠을 때 재생할 사운드 */
	UPROPERTY(EditAnywhere, Category = PickableItem)
	class USoundBase* PickupSound;
};
