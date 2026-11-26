#include "SuperSideScroller_Player.h"

#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnemyBase.h"
#include "PlayerProjectile.h"

namespace
{
	template <typename T>
	T* LoadIfNull(T* Current, const TCHAR* Path)
	{
		return Current ? Current : LoadObject<T>(nullptr, Path);
	}
}

ASuperSideScroller_Player::ASuperSideScroller_Player()
{
	// 기본적으로 전력 질주 동작을 false로 설정
	bIsSprinting = false;
	// 실습 15.03 / 15.06
	NumberofCollectables = 0;
	bHasPowerupActive = false;
	// MaxWalkSpeed 값을 300.0f로 설정
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	IC_Character = nullptr;
	IA_Sprint = nullptr;
	IA_Throw = nullptr;
	ThrowMontage = nullptr;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASuperSideScroller_Player::OnCapsuleHit);
}

void ASuperSideScroller_Player::BeginPlay()
{
	Super::BeginPlay();
	SpawnTransform = GetActorTransform();

	// 블루프린트에서 지정하지 않았다면 기본 경로의 애셋을 사용
	ThrowMontage = LoadIfNull(ThrowMontage, TEXT("/Game/MainCharacter/Animation/AM_Throw.AM_Throw"));
	if (!PlayerProjectile)
	{
		PlayerProjectile = LoadClass<APlayerProjectile>(nullptr,
			TEXT("/Game/MainCharacter/Projectile/BP_PlayerProjectile.BP_PlayerProjectile_C"));
		if (!PlayerProjectile)
		{
			PlayerProjectile = APlayerProjectile::StaticClass();
		}
	}
}

void ASuperSideScroller_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 항상 필요하지는 않지만, Super 키워드를 사용해 부모 클래스의 함수를 호출하는 것이 좋습니다.
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	IC_Character = LoadIfNull(IC_Character, TEXT("/Game/Input/IC_SideScrollerCharacter.IC_SideScrollerCharacter"));
	IA_Sprint = LoadIfNull(IA_Sprint, TEXT("/Game/Input/Sprint/IA_Sprint.IA_Sprint"));
	IA_Throw = LoadIfNull(IA_Throw, TEXT("/Game/Input/Throw/IA_Throw.IA_Throw"));

	UEnhancedInputComponent* EnhancedPlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedPlayerInput)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		UEnhancedInputLocalPlayerSubsystem* EnhancedSubsystem = PlayerController
			? ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())
			: nullptr;

		if (EnhancedSubsystem)
		{
			// 이동/점프 (템플릿 IMC_Default)
			if (IC_Default)
			{
				EnhancedSubsystem->AddMappingContext(IC_Default, 0);
			}
			// 전력 질주/던지기 (IC_SideScrollerCharacter)
			if (IC_Character)
			{
				EnhancedSubsystem->AddMappingContext(IC_Character, 1);
			}
		}

		if (IA_Sprint)
		{
			// Sprint 입력 액션의 누름 이벤트를 Sprint 함수에 바인딩합니다
			EnhancedPlayerInput->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &ASuperSideScroller_Player::Sprint);
			// Sprint 입력 액션의 해제 이벤트를 StopSprinting 함수에 바인딩합니다
			EnhancedPlayerInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASuperSideScroller_Player::StopSprinting);
		}

		if (IA_Throw)
		{
			// Throw 입력 액션을 ThrowProjectile 함수에 바인딩합니다
			EnhancedPlayerInput->BindAction(IA_Throw, ETriggerEvent::Triggered, this, &ASuperSideScroller_Player::ThrowProjectile);
		}
	}
}

void ASuperSideScroller_Player::Sprint()
{
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		// 실습 15.06: 파워업 중에는 더 빠르게 질주
		if (bHasPowerupActive)
		{
			GetCharacterMovement()->MaxWalkSpeed = 900.0f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		}
	}
}

void ASuperSideScroller_Player::StopSprinting()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		// 실습 15.06: 파워업 중에는 걷기 속도도 500
		if (bHasPowerupActive)
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 300.0f;
		}
	}
}

void ASuperSideScroller_Player::ThrowProjectile()
{
	// 실습 14.03: 던지기 몽타주가 재생 중이 아닐 때만 재생합니다.
	// 실제 발사체 생성은 몽타주의 Anim_ProjectileNotify 가 SpawnProjectile() 을 호출해서 이루어집니다.
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (ThrowMontage && AnimInstance)
	{
		const bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(ThrowMontage);
		if (!bIsMontagePlaying)
		{
			AnimInstance->Montage_Play(ThrowMontage, 1.0f);
		}
		return;
	}

	// 몽타주를 재생할 수 없는 경우에는 바로 발사체를 생성
	SpawnProjectile();
}

void ASuperSideScroller_Player::SpawnProjectile()
{
	if (PlayerProjectile)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			// 던진 플레이어와 충돌하지 않도록 Instigator 지정
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// 실습 14.04 / 14.05: 오른손의 ProjectileSocket 위치에서 생성
			FVector SpawnLocation;
			if (GetMesh()->DoesSocketExist(FName("ProjectileSocket")))
			{
				SpawnLocation = this->GetMesh()->GetSocketLocation(FName("ProjectileSocket"));
			}
			else
			{
				SpawnLocation = GetActorLocation() + GetActorForwardVector() * 80.f + FVector(0.f, 0.f, 40.f);
			}
			// 횡스크롤 게임이므로 캐릭터와 같은 평면(X)에서 날아가도록 고정
			SpawnLocation.X = GetActorLocation().X;

			const FRotator Rotation = GetActorForwardVector().Rotation();

			APlayerProjectile* Projectile = World->SpawnActor<APlayerProjectile>(PlayerProjectile,
				SpawnLocation, Rotation, SpawnParams);
			if (Projectile)
			{
				// 플레이어가 움직이면서 자신의 발사체에 부딪히지 않도록
				GetCapsuleComponent()->IgnoreActorWhenMoving(Projectile, true);
				Projectile->CollisionComp->IgnoreActorWhenMoving(this, true);
			}
		}
	}
}

void ASuperSideScroller_Player::IncrementNumberofCollectables(int32 Value)
{
	if (Value <= 0)
	{
		return;
	}
	else
	{
		NumberofCollectables += Value;
	}

	UE_LOG(LogTemp, Warning, TEXT("Number of Coins: %d"), NumberofCollectables);
}

void ASuperSideScroller_Player::IncreaseMovementPowerup()
{
	bHasPowerupActive = true;

	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 900.0f : 500.0f;
	GetCharacterMovement()->JumpZVelocity = 1500.0f;

	UWorld* World = GetWorld();
	if (World)
	{
		// 8초 후 EndPowerup() 호출 (반복 없음). 이미 파워업 중이면 타이머가 다시 8초로 갱신됩니다.
		World->GetTimerManager().SetTimer(PowerupHandle, this, &ASuperSideScroller_Player::EndPowerup, 8.0f, false);
	}
}

void ASuperSideScroller_Player::EndPowerup()
{
	bHasPowerupActive = false;

	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? 500.0f : 300.0f;
	GetCharacterMovement()->JumpZVelocity = 1000.0f;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(PowerupHandle);
	}
}

void ASuperSideScroller_Player::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AEnemyBase>(OtherActor))
	{
		OnTouchedEnemy(OtherActor);
	}
}

void ASuperSideScroller_Player::OnTouchedEnemy(AActor* Enemy)
{
	if (!Enemy || Enemy->IsActorBeingDestroyed())
	{
		return;
	}

	// 떨어지면서 적의 머리 위를 밟으면 적을 제거하고 튀어오름
	const bool bFalling = GetCharacterMovement()->IsFalling() && GetVelocity().Z < 0.f;
	const bool bAbove = GetActorLocation().Z > Enemy->GetActorLocation().Z + 60.f;
	if (bFalling && bAbove)
	{
		if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(Enemy))
		{
			EnemyBase->DestroyEnemy();
		}
		else
		{
			Enemy->Destroy();
		}
		LaunchCharacter(FVector(0.f, 0.f, StompBounceVelocity), false, true);
		return;
	}

	// 그 외에는 적에게 피해를 입음 -> 시작 위치에서 다시 시작
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastDamageTime > 1.0)
	{
		LastDamageTime = Now;
		UE_LOG(LogTemp, Warning, TEXT("Player hit by enemy: %s"), *Enemy->GetName());
		Respawn();
	}
}

void ASuperSideScroller_Player::FellOutOfWorld(const UDamageType& DamageType)
{
	// 레벨 아래로 떨어지면 액터를 파괴하는 대신 시작 위치로 되돌림
	Respawn();
}

void ASuperSideScroller_Player::Respawn()
{
	GetCharacterMovement()->StopMovementImmediately();
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	StopSprinting();
}
