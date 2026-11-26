#pragma once

#include "CoreMinimal.h"
#include "SuperSideScrollerCharacter.h"
#include "SuperSideScroller_Player.generated.h"

class UInputAction;
class UInputMappingContext;
class UAnimMontage;
class APlayerProjectile;

/**
 * 실습 11.04 ~ 11.06, 활동 11.03, 실습 12.06: 플레이어 캐릭터
 * - Shift 를 누르고 있으면 전력 질주 (MaxWalkSpeed 300 -> 500)
 * - 마우스 왼쪽 버튼 / 게임패드 우측 트리거로 던지기 (AM_Throw 몽타주 + 발사체)
 * - 적과 부딪히면 피해 (시작 위치에서 다시 시작), 적을 위에서 밟으면 적 제거
 * - 실습 14.03 / 14.05: 던지기 몽타주를 재생하고, 애님 노티파이 시점에 ProjectileSocket 에서 발사체 생성
 * - 실습 15.03: 수집한 코인 개수 추적 (NumberofCollectables)
 * - 실습 15.06: 물약 파워업 (8초 동안 이동 속도 / 점프 높이 증가)
 */
UCLASS()
class ASSIGNMENT_PROJECT_API ASuperSideScroller_Player : public ASuperSideScrollerCharacter
{
	GENERATED_BODY()

public:
	// 생성자
	ASuperSideScroller_Player();

	/** 적과 충돌했을 때 호출 */
	void OnTouchedEnemy(AActor* Enemy);

	virtual void FellOutOfWorld(const UDamageType& DamageType) override;

	// 전력 질주
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Sprint();
	// 전력 질주 정지
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopSprinting();
	// 발사체 던지기 (활동 11.03)
	UFUNCTION(BlueprintCallable, Category = "Player")
	void ThrowProjectile();

	/** 실습 14.05: ProjectileSocket 위치에서 발사체를 생성 (Anim_ProjectileNotify 에서 호출) */
	void SpawnProjectile();

	/** 실습 15.03: 현재까지 수집한 코인 개수 (UMG 에서 사용) */
	UFUNCTION(BlueprintPure)
	int32 GetCurrentNumberofCollectables() { return NumberofCollectables; };

	/** 실습 15.03: 수집한 코인 개수 증가 (0 이하의 값은 무시) */
	void IncrementNumberofCollectables(int32 Value);

	/** 실습 15.06: 물약 파워업 효과 활성화 */
	void IncreaseMovementPowerup();

protected:
	virtual void BeginPlay() override;

	// 플레이어 입력 컴포넌트를 설정하기 위해 부모 캐릭터 클래스의 함수를 재정의합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* IC_Character;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Sprint;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* IA_Throw;

	/** 실습 12.06: 던지기 애니메이션 몽타주 (AM_Throw) */
	UPROPERTY(EditAnywhere, Category = "Throw")
	UAnimMontage* ThrowMontage;

	/** 던질 발사체 클래스 (BP_PlayerProjectile) */
	UPROPERTY(EditAnywhere, Category = "Throw")
	TSubclassOf<APlayerProjectile> PlayerProjectile;

	/** 적을 밟았을 때 튀어오르는 속도 */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float StompBounceVelocity = 900.f;

	/** 실습 15.06: 파워업 효과 종료 (타이머 콜백) */
	void EndPowerup();

	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

private:
	// 전력 질주 중인지 확인하는 bool 변수. 안전장치.
	bool bIsSprinting;

	/** 실습 15.03: 플레이어가 현재 수집한 코인의 수 */
	int32 NumberofCollectables;

	/** 실습 15.06: 파워업 지속 시간을 관리하는 타이머 핸들 */
	FTimerHandle PowerupHandle;

	/** 실습 15.06: 파워업 활성화 여부 */
	bool bHasPowerupActive;

	void Respawn();

	FTransform SpawnTransform;
	double LastDamageTime = -100.0;
};
