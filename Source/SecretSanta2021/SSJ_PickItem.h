// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSJ_PickItem.generated.h"

UCLASS()
class SECRETSANTA2021_API ASSJ_PickItem : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASSJ_PickItem();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class USphereComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* DisplayMesh;

	void DestroyThisItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Timer to delay collision on spawning, preventing player from picking up dropped item
	FTimerHandle FOnSpawnTimerHandle;

	UFUNCTION()
	void TurnCollisionBackOn() const;
	
};
