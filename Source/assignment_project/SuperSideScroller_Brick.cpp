#include "SuperSideScroller_Brick.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "SuperSideScroller_Player.h"

ASuperSideScroller_Brick::ASuperSideScroller_Brick()
{
	// 실습 15.07: Tick 은 사용하지 않음
	PrimaryActorTick.bCanEverTick = false;

	BrickMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrickMesh"));
	BrickMesh->SetCollisionProfileName("BlockAll");
	RootComponent = BrickMesh;

	// 기본 메시: 엔진 큐브 (100 x 100 x 100, 피벗 중앙)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BrickMesh->SetStaticMesh(CubeMesh.Object);
	}

	BrickCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BrickCollision"));
	BrickCollision->SetCollisionProfileName("BlockAll");
	BrickCollision->SetupAttachment(RootComponent);
	// 벽돌 아랫면에서 조금만 튀어나오도록 배치 (플레이어가 아래에서 점프해 머리로 칠 때만 닿음)
	// 엔진 큐브는 피벗이 중앙이므로, 교재의 Z=30 (아래쪽이 피벗인 Shape_Cube 기준) 대신 아래쪽에 둡니다.
	BrickCollision->SetBoxExtent(FVector(40.f, 40.f, 10.f));
	BrickCollision->SetRelativeLocation(FVector(0.f, 0.f, -50.f));

	BrickCollision->OnComponentHit.AddDynamic(this, &ASuperSideScroller_Brick::OnHit);

	bHasCollectable = false;
	HitSound = nullptr;
	Explosion = nullptr;
}

void ASuperSideScroller_Brick::BeginPlay()
{
	Super::BeginPlay();
}

void ASuperSideScroller_Brick::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ASuperSideScroller_Player* Player = Cast<ASuperSideScroller_Player>(OtherActor);
	if (Player && bHasCollectable && !IsActorBeingDestroyed())
	{
		AddCollectable(Player);
		PlayHitSound();
		PlayHitExplosion();
		Destroy();
	}
}

void ASuperSideScroller_Brick::AddCollectable(ASuperSideScroller_Player* Player)
{
	Player->IncrementNumberofCollectables(CollectableValue);
}

void ASuperSideScroller_Brick::PlayHitSound()
{
	UWorld* World = GetWorld();
	if (World && HitSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(World, HitSound, GetActorLocation());
	}
}

void ASuperSideScroller_Brick::PlayHitExplosion()
{
	UWorld* World = GetWorld();
	if (World && Explosion)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, Explosion, GetActorTransform());
	}
}
