#pragma once

#include "CoreMinimal.h"
#include "PickableActor_Base.h"
#include "PickableActor_Powerup.generated.h"

/**
 * 활동 15.03: 물약 파워업 아이템
 * - 플레이어가 주우면 IncreaseMovementPowerup() 으로 8초간 이동 속도 / 점프 높이 증가
 */
UCLASS()
class ASSIGNMENT_PROJECT_API APickableActor_Powerup : public APickableActor_Base
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void PlayerPickedUp(class ASuperSideScroller_Player* Player) override;
};
