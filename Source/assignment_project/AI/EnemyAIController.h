#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;

/**
 * 실습 13.01 / 13.04: 적 AI 컨트롤러 (BP_AIControllerEnemy 의 부모 클래스)
 * - 빙의(Possess) 시 Run Behavior Tree 로 BT_EnemyAI 를 실행합니다.
 * - BT_EnemyAI 애셋에 아직 노드가 없으면, 실습 13.06 과 같은 구조의 트리
 *   (Sequence -> BTTask_FindLocation -> Move To(50) -> Wait) 를 코드로 만들어 실행합니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	/** 실행할 비헤이비어 트리 (BT_EnemyAI) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	/** Move To 이후 대기 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolWaitTime = 1.0f;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UBehaviorTree* BuildPatrolBehaviorTree();

	UPROPERTY(Transient)
	TObjectPtr<UBehaviorTree> RuntimeTree;
};
