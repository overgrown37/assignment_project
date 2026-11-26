#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindLocation.generated.h"

/**
 * 실습 13.05 / 13.10: 적이 이동할 다음 위치를 찾아 블랙보드(NewLocation 키)에 저장하는 태스크.
 * - 적에게 Patrol Points(BP_AIPoints)가 지정되어 있으면 GetNextPoint() 사용
 * - 없으면 GetRandomLocationInNavigableRadius(반경 1000) 사용
 */
UCLASS(meta = (DisplayName = "Find Location (C++)"))
class ASSIGNMENT_PROJECT_API UBTTask_FindLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindLocation();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NewLocation;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float SearchRadius = 1000.f;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
