#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//������ ���̴�
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	//���濡�� ���̴�
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_FireAnimation;


	//ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

public:
	AFP_FirstPersonCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponDamage;


protected:
	void OnFire();
	void OnTestAction();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


public:
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return FP_Mesh; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return Camera; }


	/////////////////////
private:
	UFUNCTION(Reliable, Server) //only server
		void OnServer();

	//Todo
	//UFUNCTION(Reliable, Client)
	//UFUNCTION(Reliable, NetMulticast)
	// -> �ǽ��� �����Ͽ� �սô�.
};

