#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SuperSideScroller_Controller.generated.h"

class UCoinCollectionWidget;

/**
 * 실습 15.05: 플레이어 화면에 코인 카운터 UI 를 추가하는 플레이어 컨트롤러 (BP_SuperSideScroller_PC 의 부모)
 * - Event On Possess -> Cast to SuperSideScroller_Player -> Create Widget(BP_UI_CoinCollection, Player) -> Add to Viewport
 */
UCLASS()
class ASSIGNMENT_PROJECT_API ASuperSideScroller_Controller : public APlayerController
{
	GENERATED_BODY()

public:
	/** 화면에 표시할 코인 카운터 위젯 클래스 (BP_UI_CoinCollection) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCoinCollectionWidget> CoinCollectionWidgetClass;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UCoinCollectionWidget> CoinCollectionWidget;
};
