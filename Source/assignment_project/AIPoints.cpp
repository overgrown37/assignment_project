#include "AIPoints.h"

#include "Components/BillboardComponent.h"
#include "Components/SceneComponent.h"

AAIPoints::AAIPoints()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

#if WITH_EDITORONLY_DATA
	Sprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (Sprite)
	{
		Sprite->SetupAttachment(RootComponent);
	}
#endif
}

FVector AAIPoints::GetNextPoint() const
{
	if (Points.Num() == 0)
	{
		return GetActorLocation();
	}

	// Random Integer(Length) -> Get(a copy) -> Transform Location(GetActorTransform)
	const int32 Index = FMath::RandRange(0, Points.Num() - 1);
	return GetActorTransform().TransformPosition(Points[Index]);
}
