#include "AI/EnemyAIController.h"

#include "AI/BTTask_FindLocation.h"
#include "AI/BTTask_MoveToLocation.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBehaviorTree* TreeToRun = BehaviorTree;
	if (!TreeToRun)
	{
		TreeToRun = LoadObject<UBehaviorTree>(nullptr, TEXT("/Game/Enemy/AI/BT_EnemyAI.BT_EnemyAI"));
	}

	// 애셋의 루트 아래에 노드가 없으면 코드로 만든 정찰 트리를 사용
	if (!TreeToRun || !TreeToRun->RootNode)
	{
		if (!RuntimeTree)
		{
			RuntimeTree = BuildPatrolBehaviorTree();
		}
		TreeToRun = RuntimeTree;
	}

	if (TreeToRun)
	{
		RunBehaviorTree(TreeToRun);
	}
}

UBehaviorTree* AEnemyAIController::BuildPatrolBehaviorTree()
{
	static const FName MoveToLocationKey(TEXT("MoveToLocation"));

	// Blackboard: BB_EnemyAI 애셋에 MoveToLocation 키가 있으면 사용, 없으면 런타임에 생성
	UBlackboardData* BBAsset = LoadObject<UBlackboardData>(nullptr, TEXT("/Game/Enemy/AI/BB_EnemyAI.BB_EnemyAI"));
	if (!BBAsset || BBAsset->GetKeyID(MoveToLocationKey) == FBlackboard::InvalidKey)
	{
		BBAsset = NewObject<UBlackboardData>(this, TEXT("BB_EnemyAI_Runtime"));
		FBlackboardEntry Entry;
		Entry.EntryName = MoveToLocationKey;
		Entry.KeyType = NewObject<UBlackboardKeyType_Vector>(BBAsset);
		BBAsset->Keys.Add(Entry);
		BBAsset->UpdateKeyIDs();
	}

	UBehaviorTree* Tree = NewObject<UBehaviorTree>(this, TEXT("BT_EnemyAI_Runtime"));
	Tree->BlackboardAsset = BBAsset;

	// Root -> Sequence
	UBTComposite_Sequence* Sequence = NewObject<UBTComposite_Sequence>(Tree);

	// 1) BTTask_FindLocation: 정찰 지점(또는 내비 메시 위 임의 위치)을 MoveToLocation 에 저장
	UBTTask_FindLocation* FindLocation = NewObject<UBTTask_FindLocation>(Tree);

	// 2) Move To: MoveToLocation 으로 이동 (Acceptable Radius 50)
	UBTTask_MoveToLocation* MoveTo = NewObject<UBTTask_MoveToLocation>(Tree);

	// 3) Wait: 잠시 멈춘 후 다시 반복
	UBTTask_Wait* Wait = NewObject<UBTTask_Wait>(Tree);
	Wait->WaitTime = PatrolWaitTime;

	const TArray<UBTTaskNode*> Tasks = { FindLocation, MoveTo, Wait };
	for (UBTTaskNode* Task : Tasks)
	{
		FBTCompositeChild Child;
		Child.ChildTask = Task;
		Sequence->Children.Add(Child);
	}

	Tree->RootNode = Sequence;
	return Tree;
}
