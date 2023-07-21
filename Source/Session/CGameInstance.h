#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Widgets/IMenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CGameInstance.generated.h"

UCLASS()
class SESSION_API UCGameInstance : public UGameInstance, public IIMenuInterface
{
	GENERATED_BODY()

public:
	UCGameInstance(const FObjectInitializer& InObject);
	virtual void Init() override;

public:
	UFUNCTION(Exec)
		void Host(const FString& InSessionName) override;

	UFUNCTION(Exec)
		void Join(uint32 InIndex) override;

	UFUNCTION(BlueprintCallable, Exec)
		void LoadMainMenu();

	UFUNCTION(BlueprintCallable, Exec)
		void LoadGameMenu();

	void ReturnToMainMenu() override;
	void RefreshSessionList() override;

	void StartSession();

private:
	void OnCreateSessionComplete(FName InSessionName, bool InSuccess);
	void OnDestroySessionComplete(FName InSessionName, bool InSuccess);
	void OnFindSessionComplete(bool InSuccess);
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type InResult);
	void OnNetworkFailure(UWorld* InWorld, UNetDriver* InNetDriver, ENetworkFailure::Type InFaliureType, const FString& OutErrorMessge);

	void CreateSession();

private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	TSubclassOf<UUserWidget> GameMenuWidgetClass;

	class UCMainMenu* MainMenu;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString DesiredSessionName;
};
