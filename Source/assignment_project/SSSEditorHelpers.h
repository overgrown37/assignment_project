#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SSSEditorHelpers.generated.h"

/**
 * 에디터 자동화 스크립트(Content/Python/setup_ch14_15.py) 에서 사용하는 보조 함수.
 * 실습 14.04 / 14.11 의 스켈레톤 소켓(ProjectileSocket, EffectSocket) 을 추가합니다.
 * (USkeletalMeshSocket::SocketName 은 파이썬에서 읽기 전용이라 C++ 로 처리)
 */
UCLASS()
class ASSIGNMENT_PROJECT_API USSSEditorHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SuperSideScroller|Editor")
	static bool AddSkeletonSocket(USkeleton* Skeleton, FName SocketName, FName BoneName, FVector RelativeLocation);
};
