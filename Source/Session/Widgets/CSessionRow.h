#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSessionRow.generated.h"

UCLASS()
class SESSION_API UCSessionRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(class UCMainMenu* InParent, uint32 InIndex);

private:
	UFUNCTION()
		void OnClicked();
	
public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SessionName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HostUserName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ConnectionFraction;

public:
	UPROPERTY(BlueprintReadOnly)
		bool bSelected;

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* RowButton;

private:
	UPROPERTY()
		class UCMainMenu* Parent;
	
	uint32 Index;
};
