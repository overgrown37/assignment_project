#include "Anim_ProjectileNotify.h"

#include "Components/SkeletalMeshComponent.h"
#include "SuperSideScroller_Player.h"

void UAnim_ProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 실습 14.06: 메시의 소유자가 플레이어라면 발사체를 생성
	if (!MeshComp)
	{
		return;
	}

	ASuperSideScroller_Player* Player = Cast<ASuperSideScroller_Player>(MeshComp->GetOwner());
	if (Player)
	{
		Player->SpawnProjectile();
	}
}
