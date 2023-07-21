#pragma once

#include "CoreMinimal.h"
#include "Widgets/CMenuBase.h"
#include "CGameMenu.generated.h"

UCLASS()
class SESSION_API UCGameMenu : public UCMenuBase
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
		void CancelButtonPressed();

	UFUNCTION()
		void QuitButtonPressed();

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;
};
