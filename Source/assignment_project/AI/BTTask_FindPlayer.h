#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPlayer.generated.h"

/**
 * 활동 13.02: 플레이어 위치(Get Player Character(0) -> Get Actor Location)를
 * 블랙보드 NewLocation 키에 저장하는 태스크.
 */
UCLASS(meta = (DisplayName = "Find Player (C++)"))
class ASSIGNMENT_PROJECT_API UBTTask_FindPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPlayer();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NewLocation;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
