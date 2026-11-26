#include "PlayerProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyBase.h"

APlayerProjectile::APlayerProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("BlockAll");
	CollisionComp->OnComponentHit.AddDynamic(this, &APlayerProjectile::OnHit);

	// 루트 컴포넌트로 설정
	RootComponent = CollisionComp;

	// 이 발사체의 움직임을 제어하려면 ProjectileMovementComponent를 사용합니다.
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 800.0f;
	ProjectileMovement->MaxSpeed = 800.0f;

	// 스태틱 메시 컴포넌트 (UE5 에서는 생성자에서 SetupAttachment 사용)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(SphereMesh.Object);
	}
	// 지름 100 의 구를 콜리전 반경(15)에 맞게 축소
	MeshComp->SetRelativeScale3D(FVector(0.3f));

	// 실습 14.10: 이동 사운드 / 파티클 이펙트 컴포넌트를 루트 컴포넌트에 부착
	// (UE5 의 생성자에서는 AttachToComponent 대신 SetupAttachment 를 사용합니다.)
	ProjectileMovementSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ProjectileMovementSound"));
	ProjectileMovementSound->SetupAttachment(RootComponent);

	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile Effect"));
	ProjectileEffect->SetupAttachment(RootComponent);

	DestroyEffect = nullptr;
	DestroySound = nullptr;

	InitialLifeSpan = 3.0f;
}

void APlayerProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 던진 플레이어와는 충돌하지 않도록
	if (APawn* InstigatorPawn = GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(InstigatorPawn, true);
	}
}

void APlayerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 던진 플레이어 자신과의 충돌은 무시
	if (OtherActor && OtherActor == GetInstigator())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("HIT %s"), *GetNameSafe(OtherActor));

	// 활동 14.01: 한 번의 타격으로 적을 파괴 (DestroyEnemy 로 VFX/SFX 재생)
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (Enemy)
	{
		Enemy->DestroyEnemy();
	}

	// 어떤 표면에 부딪혀도 발사체는 폭발 (튕기지 않음)
	ExplodeProjectile();
}

void APlayerProjectile::ExplodeProjectile()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;

	// 활동 14.02: 발사체가 파괴될 때 VFX / SFX 를 스폰
	UWorld* World = GetWorld();
	if (World)
	{
		if (DestroyEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, DestroyEffect, GetActorTransform());
		}
		if (DestroySound)
		{
			UGameplayStatics::SpawnSoundAtLocation(World, DestroySound, GetActorLocation());
		}
	}

	// 실습 14.08
	Destroy();
}
