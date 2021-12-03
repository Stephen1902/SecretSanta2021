// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SSJ_GameCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnCharacterSwitch, int32);

UCLASS()
class SECRETSANTA2021_API ASSJ_GameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASSJ_GameCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UCameraComponent* CameraComp;

	// Colour the skin of this character is set to (default unreal mannequin only)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	FLinearColor ColourOfMesh;

	// Displays the mesh for any item picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup")
	UStaticMeshComponent* ItemDisplayMesh;
	
	void SetInteractingItem(class ASSJ_LevelInteractable* InteractingItemIn, bool InteractingWithTrigger);

	void SetPickupItem(class ASSJ_PickItem* PickupItemIn);

	void SetLiftBeingOverlapped(class ASSJ_LevelLift* LevelLiftIn, bool IsLowerLevel);

	UFUNCTION()
	class ASSJ_PickItem* GetItemBeingHeld() const;

	void SetAIControllerRef();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynMat;

	UPROPERTY()
	class ASSJ_GameStateBase* GameStateBaseRef;

	void SwitchCharacters(int32 CharacterIndexToSwitchTo);
	
	UFUNCTION()
	void MoveRight(float AxisVal);

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void PickupDropItem();

	// Class of interactable that this character is currently overlapping
	UPROPERTY()
	class ASSJ_LevelInteractable* CurrentInteractingItem;

	// Boolean to check if overlapped item is a trigger or triggered
	bool bIsInteractingWithTrigger;	

	// World items that the player can pick up
	UPROPERTY()
	class ASSJ_PickItem* ItemToBePickedUp;	

	// World items that the player is currently holding
	UPROPERTY()
	class ASSJ_PickItem* ItemBeingHeld;

	// Item in the characters hand
	UPROPERTY()
	TSubclassOf<class ASSJ_PickItem> ItemToSpawn;

	// Static mesh to be used when holding an item or adding it to a trigger
	UPROPERTY()
	UStaticMesh* MeshToCreate;

	// Lift reference for the character to use to change floors
	UPROPERTY()
	class ASSJ_LevelLift* LiftBeingOverlapped;

	bool bLowerLevelOverlapped;
	
	UPROPERTY()
	class ASSJ_AIController* AIControllerRef;

	UPROPERTY()
	FTimerHandle GetControllerHandle;
};
