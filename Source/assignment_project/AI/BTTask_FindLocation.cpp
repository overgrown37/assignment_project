#include "AI/BTTask_FindLocation.h"

#include "AIController.h"
#include "AIPoints.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "EnemyBase.h"
#include "NavigationSystem.h"

UBTTask_FindLocation::UBTTask_FindLocation()
{
	NodeName = TEXT("BTTask_FindLocation");
	NewLocation.SelectedKeyName = TEXT("MoveToLocation");
	NewLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindLocation, NewLocation));
}

void UBTTask_FindLocation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		NewLocation.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_FindLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Pawn || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	FVector Location = FVector::ZeroVector;
	bool bFound = false;

	// 실습 13.10: Controlled Pawn -> BP_Enemy 로 캐스팅 -> Patrol Points -> GetNextPoint
	const AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);
	if (Enemy && Enemy->PatrolPoints && Enemy->PatrolPoints->Points.Num() > 0)
	{
		Location = Enemy->PatrolPoints->GetNextPoint();
		bFound = true;
	}
	else if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Pawn->GetWorld()))
	{
		// 실습 13.05: GetRandomLocationInNavigableRadius(Origin = GetActorLocation, Radius = 1000)
		FNavLocation NavLocation;
		if (NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), SearchRadius, NavLocation))
		{
			Location = NavLocation.Location;
			bFound = true;
		}
	}

	if (!bFound)
	{
		return EBTNodeResult::Failed;
	}

	// 횡스크롤 게임이므로 X 값은 적의 현재 위치로 고정 (Y-Z 평면에서만 이동)
	Location.X = Pawn->GetActorLocation().X;

	Blackboard->SetValueAsVector(NewLocation.SelectedKeyName, Location);
	return EBTNodeResult::Succeeded;
}
