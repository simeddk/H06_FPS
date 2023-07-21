#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTriggerButton.generated.h"

UCLASS()
class SESSION_API ACTriggerButton : public AActor
{
	GENERATED_BODY()
	
public:	
	ACTriggerButton();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly)
		class UBoxComponent* Box;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* Mesh;

private:
	UPROPERTY(EditInstanceOnly)
		TArray<class ACMovingPlatform*> Platforms;

};
