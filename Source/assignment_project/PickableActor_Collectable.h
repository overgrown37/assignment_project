#pragma once

#include "CoreMinimal.h"
#include "PickableActor_Base.h"
#include "PickableActor_Collectable.generated.h"

/**
 * 실습 15.02 / 활동 15.02 / 실습 15.03: 코인 수집용 아이템
 * - 플레이어가 주우면 CollectableValue 만큼 코인 수를 증가시킵니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API APickableActor_Collectable : public APickableActor_Base
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PlayerPickedUp(class ASuperSideScroller_Player* Player) override;

public:
	UPROPERTY(EditAnywhere, Category = Collectable)
	int32 CollectableValue = 1;
};
