#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Anim_ProjectileNotify.generated.h"

/**
 * 실습 14.01 / 14.06: 던지기 애니메이션의 특정 프레임에서 플레이어 발사체를 생성하는 애님 노티파이
 * - AM_Throw 몽타주의 노티파이 트랙에 배치되어, 해당 시점에 SpawnProjectile() 을 호출합니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API UAnim_ProjectileNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
