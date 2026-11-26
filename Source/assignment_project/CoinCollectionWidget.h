#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoinCollectionWidget.generated.h"

class UTextBlock;

/**
 * 실습 15.04: 코인 카운터 UI (BP_UI_CoinCollection 의 부모 클래스)
 * - 캔버스 패널 왼쪽 위(앵커) 에서 Position (100, 100), Size To Content, 글꼴 크기 48 인 Text 위젯
 * - Text 는 GetNumberofCollectables() 에 바인딩되어 "Coins: {coins}" 형식으로 표시됩니다.
 * - 디자이너에서 위젯을 직접 배치하지 않은 경우, 위 구성을 코드에서 자동으로 만듭니다.
 */
UCLASS()
class ASSIGNMENT_PROJECT_API UCoinCollectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 코인 수를 읽어올 플레이어 (Instance Editable + Expose on Spawn) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<class ASuperSideScroller_Player> Player;

	/** Text 바인딩 함수: Format Text "Coins: {coins}" */
	UFUNCTION(BlueprintPure, Category = "Coin")
	FText GetNumberofCollectables() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CoinText;
};
