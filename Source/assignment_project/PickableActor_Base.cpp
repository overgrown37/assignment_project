#include "PickableActor_Base.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "SuperSideScroller_Player.h"

APickableActor_Base::APickableActor_Base()
{
	// 실습 15.01: Tick 은 사용하지 않음
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(30.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 실습 15.01 의 Shape_Cone 대신 엔진 기본 원뿔 메시를 기본값으로 사용
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(ConeMesh.Object);
		MeshComp->SetRelativeScale3D(FVector(0.5f));
	}

	RotationComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotationComp"));

	PickupSound = nullptr;

	// 활동 15.01: 오버랩 이벤트 바인딩
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickableActor_Base::BeginOverlap);
}

void APickableActor_Base::BeginPlay()
{
	Super::BeginPlay();
}

void APickableActor_Base::BeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASuperSideScroller_Player* Player = Cast<ASuperSideScroller_Player>(OtherActor);
	if (Player && !IsActorBeingDestroyed())
	{
		PlayerPickedUp(Player);
	}
}

void APickableActor_Base::PlayerPickedUp(ASuperSideScroller_Player* Player)
{
	UWorld* World = GetWorld();
	if (World && PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(World, PickupSound, GetActorLocation());
	}

	Destroy();
}
