#include "PickableActor_Collectable.h"

#include "SuperSideScroller_Player.h"

void APickableActor_Collectable::BeginPlay()
{
	Super::BeginPlay();
}

void APickableActor_Collectable::PlayerPickedUp(ASuperSideScroller_Player* Player)
{
	// 실습 15.03: 플레이어의 코인 수 증가
	Player->IncrementNumberofCollectables(CollectableValue);

	// 부모: PickupSound 재생 후 액터 파괴
	Super::PlayerPickedUp(Player);
}
