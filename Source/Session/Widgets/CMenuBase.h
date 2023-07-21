#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/IMenuInterface.h"
#include "CMenuBase.generated.h"

UCLASS()
class SESSION_API UCMenuBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwingGameInstance(IIMenuInterface* InOwingInstance);
	void SetUIMode();
	void SetPlayMode();

protected:
	IIMenuInterface* OwingGameInstance;
};
