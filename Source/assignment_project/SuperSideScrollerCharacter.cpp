#include "SuperSideScrollerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ASuperSideScrollerCharacter::ASuperSideScrollerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// 컨트롤러 회전을 캐릭터가 따르지 않도록 (이동 방향으로 회전)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// --- Mesh: 실습 10.01 (Yaw -90) ---
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (MannyMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(MannyMesh.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -97.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	FallbackAnimClass = FSoftClassPath(TEXT(
		"/Game/MainCharacter/Blueprints/AnimBP_SuperSideScroller_MainCharacter.AnimBP_SuperSideScroller_MainCharacter_C"));

	// --- Camera Boom: 실습 10.01 ---
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);              // 캐릭터가 돌아도 카메라는 고정
	CameraBoom->SetRelativeRotation(FRotator(0.f, 180.f, 0.f)); // (Pitch=0, Yaw=180, Roll=0)
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// --- Character Movement: 실습 10.01 ---
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 720.f, 0.f);
	Move->GravityScale = 2.f;
	Move->GroundFriction = 3.f;
	Move->JumpZVelocity = 1000.f;
	Move->AirControl = 0.8f;
	Move->MaxWalkSpeed = 600.f;
	Move->MaxFlySpeed = 600.f;

	// Nav Movement: 캡슐 크기에 맞춤
	Move->NavAgentProps.AgentRadius = 42.f;
	Move->NavAgentProps.AgentHeight = 192.f;

	// Planar Movement: X 축 법선 평면에서만 이동 (Y-Z 평면)
	Move->SetPlaneConstraintEnabled(true);
	Move->SetPlaneConstraintNormal(FVector(1.f, 0.f, 0.f));

	// --- 입력 애셋 기본값 (템플릿 애셋) ---
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultIMC(TEXT("/Game/Input/IMC_Default.IMC_Default"));
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveAction(TEXT("/Game/Input/Actions/IA_Move.IA_Move"));
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpAction(TEXT("/Game/Input/Actions/IA_Jump.IA_Jump"));
	IC_Default = DefaultIMC.Object;
	IA_Move = MoveAction.Object;
	IA_Jump = JumpAction.Object;
}

void ASuperSideScrollerCharacter::BeginPlay()
{
	// 애님 클래스가 비어 있으면 기본 애니메이션 블루프린트를 지정
	if (GetMesh() && !GetMesh()->GetAnimClass() && FallbackAnimClass.IsValid())
	{
		if (UClass* AnimClass = FallbackAnimClass.TryLoadClass<UAnimInstance>())
		{
			GetMesh()->SetAnimInstanceClass(AnimClass);
		}
	}

	Super::BeginPlay();
}

void ASuperSideScrollerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Jump)
		{
			EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (IA_Move)
		{
			EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASuperSideScrollerCharacter::MoveRight);
		}
	}
}

void ASuperSideScrollerCharacter::MoveRight(const FInputActionValue& Value)
{
	// IA_Move 는 2D 축(A/D = X)이므로 X 값만 좌우 이동에 사용
	const float AxisValue = Value.Get<FVector2D>().X;
	if (!FMath::IsNearlyZero(AxisValue))
	{
		// 카메라 붐이 Yaw 180 이라 카메라는 +X 쪽에서 -X 방향을 바라봅니다.
		// 이때 화면 오른쪽은 월드 -Y 이므로 (언리얼 횡스크롤 템플릿과 동일하게) Y = -1 방향을 사용합니다.
		AddMovementInput(FVector(0.f, -1.f, 0.f), AxisValue);
	}
}
