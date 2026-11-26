#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SuperSideScrollerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * 실습 10.01: 삼인칭 템플릿 캐릭터를 횡스크롤 캐릭터로 변경한 기본 클래스.
 * - 메시 Yaw -90, 카메라 붐 Yaw 180 / Arm 500 / Socket Offset Z 75 / 충돌 테스트 끔
 * - Gravity Scale 2, Ground Friction 3, Jump Z Velocity 1000, Air Control 0.8
 * - Nav Agent Radius 42 / Height 192, Constrain to Plane (Normal X = 1)
 * - MoveRight 입력은 World Direction (0, 1, 0) 으로 Add Movement Input
 */
UCLASS(config = Game)
class ASSIGNMENT_PROJECT_API ASuperSideScrollerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASuperSideScrollerCharacter();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** 좌우 이동 (InputAxis MoveRight -> Add Movement Input, World Direction Y = 1) */
	void MoveRight(const FInputActionValue& Value);

	/** 기본 이동/점프 입력 매핑 콘텍스트 (템플릿의 IMC_Default) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IC_Default;

	/** 이동 입력 액션 (템플릿의 IA_Move, X 축 값만 사용) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	/** 점프 입력 액션 (템플릿의 IA_Jump) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	/** 메시에 애님 클래스가 지정되지 않았을 때 사용할 애니메이션 블루프린트 경로 */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FSoftClassPath FallbackAnimClass;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
};
