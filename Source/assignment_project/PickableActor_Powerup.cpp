#include "PickableActor_Powerup.h"

#include "SuperSideScroller_Player.h"

void APickableActor_Powerup::BeginPlay()
{
	Super::BeginPlay();
}

void APickableActor_Powerup::PlayerPickedUp(ASuperSideScroller_Player* Player)
{
	// 활동 15.03: 플레이어에게 파워업 효과 부여
	Player->IncreaseMovementPowerup();

	// 부모: PickupSound 재생 후 액터 파괴
	Super::PlayerPickedUp(Player);
}
