#include "SuperSideScroller_Controller.h"

#include "Blueprint/UserWidget.h"
#include "CoinCollectionWidget.h"
#include "SuperSideScroller_Player.h"

void ASuperSideScroller_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASuperSideScroller_Player* PossessedPlayer = Cast<ASuperSideScroller_Player>(InPawn);
	if (!PossessedPlayer || !IsLocalController())
	{
		return;
	}

	TSubclassOf<UCoinCollectionWidget> WidgetClass = CoinCollectionWidgetClass;
	if (!WidgetClass)
	{
		WidgetClass = LoadClass<UCoinCollectionWidget>(nullptr,
			TEXT("/Game/UI/BP_UI_CoinCollection.BP_UI_CoinCollection_C"));
		if (!WidgetClass)
		{
			WidgetClass = UCoinCollectionWidget::StaticClass();
		}
	}

	// 다시 빙의하는 경우 이전 위젯 제거
	if (CoinCollectionWidget)
	{
		CoinCollectionWidget->RemoveFromParent();
		CoinCollectionWidget = nullptr;
	}

	CoinCollectionWidget = CreateWidget<UCoinCollectionWidget>(this, WidgetClass);
	if (CoinCollectionWidget)
	{
		CoinCollectionWidget->Player = PossessedPlayer;
		CoinCollectionWidget->AddToViewport();
	}
}
