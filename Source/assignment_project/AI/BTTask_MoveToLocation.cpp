#include "AI/BTTask_MoveToLocation.h"

UBTTask_MoveToLocation::UBTTask_MoveToLocation()
{
	NodeName = TEXT("Move To");
	BlackboardKey.SelectedKeyName = TEXT("MoveToLocation");
	AcceptableRadius = 50.f;
}
