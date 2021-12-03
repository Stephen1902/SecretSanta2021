// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSJ_LevelInteractable.generated.h"

UCLASS()
class SECRETSANTA2021_API ASSJ_LevelInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASSJ_LevelInteractable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	class UBoxComponent* TriggerCollisionComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* TriggerDisplayMesh;

	// Checked if this item needs to be interacted with (ie a lever) rather than just activated (ie a pressure plate)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	bool bTriggerIsInteractedWith;

	// Amount of time trigger stays active before resetting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup", meta=(EditCondition="bTriggerIsInteractedWith"))
	float TriggerActiveTime;

	// Item needed to cause this to trigger (ie a lever)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<class ASSJ_PickItem> PickupItemNeeded;

	// Display mesh component of the item when placed on the trigger
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* TriggerItemPlacedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	class UBoxComponent* TriggeredCollisionComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* TriggeredDisplayMesh;

	// Checked if this item needs to be interacted with (ie something to pick up) rather than just activated (ie a door)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	bool bTriggeredIsInteractedWith;

	// Checked if triggered item does not reset (ie door stays open)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	bool bTriggeredIsNotReset;

	// Item given if everything is successfully triggered
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<class ASSJ_PickItem> PickupItemGiven;

	// Decides whether the item given mesh is displayed or just appears (ie a can out of a vending machine)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	bool bGivenItemDisplayed;
	
	// Mesh component to display mesh from the given item (if displayed)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* TriggeredItemGivenMesh;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
	void OnTriggerInteractionStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
	void OnTriggerInteractionEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
	void OnTriggerActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
	void OnTriggerDeactivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Triggered")
	void OnTriggeredInteractionStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Triggered")
	void OnTriggeredInteractionEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Triggered")
	void OnTriggeredActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Triggered")
	void OnTriggeredDeactivated();
	
	// If the trigger is given the item it needs to make it interact, set the mesh
	void SetTriggerStaticMesh(UStaticMesh* MeshIn);

	bool GetNeededPickupItemGiven() const { return bNeededPickupItemGiven; }

	TSubclassOf<class ASSJ_PickItem> GetNeededItemToInteract() const { return PickupItemNeeded; }

	void CallActivateTrigger();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnTriggeredOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggeredOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bNeededPickupItemGiven;
};
