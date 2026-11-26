#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPoints.generated.h"

/**
 * 실습 13.07 / 13.08: 적 정찰 위치 액터 (BP_AIPoints 의 부모 클래스)
 * - Points: 인스턴스 편집 가능 + 3D 위젯 표시 Vector 배열 (로컬 공간)
 * - GetNextPoint: 배열에서 임의의 점을 골라 월드 공간으로 변환해 반환 (순수 함수)
 */
UCLASS()
class ASSIGNMENT_PROJECT_API AAIPoints : public AActor
{
	GENERATED_BODY()

public:
	AAIPoints();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MakeEditWidget = true))
	TArray<FVector> Points;

	UFUNCTION(BlueprintPure, Category = "AI")
	FVector GetNextPoint() const;

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> Sprite;
#endif
};
