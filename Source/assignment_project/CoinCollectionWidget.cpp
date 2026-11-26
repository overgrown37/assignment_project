#include "CoinCollectionWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "SuperSideScroller_Player.h"

FText UCoinCollectionWidget::GetNumberofCollectables() const
{
	const int32 Coins = Player ? Player->GetCurrentNumberofCollectables() : 0;

	FFormatNamedArguments Args;
	Args.Add(TEXT("coins"), Coins);
	return FText::Format(NSLOCTEXT("SuperSideScroller", "CoinCounter", "Coins: {coins}"), Args);
}

TSharedRef<SWidget> UCoinCollectionWidget::RebuildWidget()
{
	// 위젯 블루프린트 디자이너가 비어 있으면 Canvas Panel + Text 위젯을 코드로 구성
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CanvasPanel"));
		WidgetTree->RootWidget = Canvas;

		CoinText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CoinText"));
		FSlateFontInfo Font = CoinText->GetFont();
		Font.Size = 48;
		CoinText->SetFont(Font);
		CoinText->SetShadowOffset(FVector2D(2.f, 2.f));
		CoinText->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.75f));

		if (UCanvasPanelSlot* CanvasSlot = Canvas->AddChildToCanvas(CoinText))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f));   // 왼쪽 위
			CanvasSlot->SetPosition(FVector2D(100.f, 100.f));
			CanvasSlot->SetAutoSize(true);                // Size To Content
		}
	}

	// Text 바인딩 (Bind -> Get Number of Collectables)
	if (CoinText)
	{
		CoinText->TextDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UCoinCollectionWidget, GetNumberofCollectables));
	}

	return Super::RebuildWidget();
}
