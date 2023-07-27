#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Game/CGameState.h"
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
	//나에게 보이는
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	//상대방에게 보이는
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_HitAnimation;

	//카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	//파티클
	UPROPERTY(VisibleAnywhere, Category = GamePlay)
		class UParticleSystemComponent* FP_GunShotParticle;

	UPROPERTY(VisibleAnywhere, Category = GamePlay)
		class UParticleSystemComponent* TP_GunShotParticle;

public:
	AFP_FirstPersonCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

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
		TSubclassOf<class ACBullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponDamage;


protected:
	virtual void BeginPlay() override;

	void OnFire();

	UFUNCTION(Reliable, Server)
		void OnServerFire(const FVector& LineStart, const FVector& LineEnd);
	void OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastShootEffects();
	void MulticastShootEffects_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayDamage();
	void PlayDamage_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayDead();
	void PlayDead_Implementation();


public:
	UFUNCTION(NetMulticast, Reliable)
		void SetTeamColor(ETeamType InTeamType);
	void SetTeamColor_Implementation(ETeamType InTeamType);

public:
	class ACPlayerState* GetSelfPlayerState();
	void SetSelfPlayerState(class ACPlayerState* NewPlayerState);

protected:
	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace);

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


public:
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return FP_Mesh; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return Camera; }


private:
	UFUNCTION()
		void Respawn();

public:
	UPROPERTY(Replicated)
		ETeamType CurrentTeam;

private:
	class UMaterialInstanceDynamic* DynamicMaterial;
	TArray<AActor*> SpawnPoints;
	class ACPlayerState* SelfPlayerState;
};

