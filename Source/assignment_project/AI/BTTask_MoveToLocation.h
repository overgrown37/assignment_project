#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToLocation.generated.h"

/**
 * 실습 13.06 의 Move To 태스크 설정을 미리 지정한 버전
 * (Blackboard Key = MoveToLocation, Acceptable Radius = 50)
 */
UCLASS(meta = (DisplayName = "Move To Location (C++)"))
class ASSIGNMENT_PROJECT_API UBTTask_MoveToLocation : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_MoveToLocation();
};
