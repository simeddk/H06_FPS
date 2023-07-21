#pragma once

#include "CoreMinimal.h"
#include "CMenuBase.h"
#include "CMainMenu.generated.h"

USTRUCT()
struct FSessionData
{
	GENERATED_BODY()

public:
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};


UCLASS()
class SESSION_API UCMainMenu : public UCMenuBase
{
	GENERATED_BODY()

public:
	UCMainMenu();

protected:
	virtual bool Initialize() override;

public:
	void SetSessionList(TArray<FSessionData> InSessionDatas);
	void SetSessionRowIndex(uint32 InIndex);

private:
	UFUNCTION()	void HostServer();
	UFUNCTION()	void JoinServer();
	UFUNCTION()	void OpenJoinMenu();
	UFUNCTION()	void OpenMainMenu();
	UFUNCTION()	void OpenHostMenu();
	UFUNCTION()	void QuitGame();

private:
	void SelectSessionRow();

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;
	
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConfirmJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CancelHostMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConfirmHostMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
		class UPanelWidget* SessionList;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SessionNameField;

private:
	TSubclassOf<class UCSessionRow> SessionRowWidgetClass;

	TOptional<uint32> SessionRowIndex;
};
