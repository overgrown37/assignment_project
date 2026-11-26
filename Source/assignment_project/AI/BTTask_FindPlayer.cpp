#include "AI/BTTask_FindPlayer.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindPlayer::UBTTask_FindPlayer()
{
	NodeName = TEXT("BTTask_FindPlayer");
	NewLocation.SelectedKeyName = TEXT("MoveToLocation");
	NewLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPlayer, NewLocation));
}

void UBTTask_FindPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		NewLocation.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_FindPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(&OwnerComp, 0);
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Player || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsVector(NewLocation.SelectedKeyName, Player->GetActorLocation());
	return EBTNodeResult::Succeeded;
}
