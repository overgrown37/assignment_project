#include "EnemyBase.h"

#include "AI/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SuperSideScroller_Player.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AEnemyBase::AEnemyBase()
{
	// Auto Possess AI: 월드에 배치 또는 생성 모두 지원
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuinnMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	if (QuinnMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(QuinnMesh.Object);
	}
	// 실습 12.08: Location (0,0,-90), Rotation Yaw -90
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));

	FallbackAnimClass = FSoftClassPath(TEXT("/Game/Enemy/Blueprints/AnimBP_Enemy.AnimBP_Enemy_C"));

	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->MaxWalkSpeed = 250.f;
	// 경로 이동 시에도 가속도를 사용해야 애님 블루프린트가 걷기 상태로 전환됩니다.
	Move->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyBase::OnCapsuleHit);

	DeathEffect = nullptr;
	DeathSound = nullptr;
}

void AEnemyBase::DestroyEnemy()
{
	// 실습 14.09: 파괴되기 전에 현재 위치에서 VFX / SFX 를 스폰
	UWorld* World = GetWorld();
	if (World)
	{
		if (DeathEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, DeathEffect, GetActorTransform());
		}
		if (DeathSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(World, DeathSound, GetActorLocation());
		}
	}

	// 실습 14.07: 액터 파괴
	Destroy();
}

void AEnemyBase::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (ASuperSideScroller_Player* Player = Cast<ASuperSideScroller_Player>(OtherActor))
	{
		Player->OnTouchedEnemy(this);
	}
}
