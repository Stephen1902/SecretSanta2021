// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SSJ_LevelLift.generated.h"

UCLASS()
class SECRETSANTA2021_API ASSJ_LevelLift : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASSJ_LevelLift();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UBoxComponent* UpperLevelCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* UpperLevelLightMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* UpperLevelDoor1Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* UpperLevelDoor2Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* UpperFloorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UBoxComponent* LowerLevelCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* LowerLevelLightMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* LowerLevelDoor1Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* LowerLevelDoor2Mesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	UStaticMeshComponent* LowerFloorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UArrowComponent* UpperFloorTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UArrowComponent* UpperLiftTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UArrowComponent* LowerFloorTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	class UArrowComponent* LowerLiftTarget;

	UPROPERTY()
	UTimelineComponent* DoorOpenTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintGetter="GetLiftIsActivated", BlueprintSetter="SetLiftActivated", Category = "Setup")
	bool bIsActivated;

	UFUNCTION(BlueprintCallable, Category = "Activate Lift")
	bool GetLiftIsActivated() const { return bIsActivated;}

	UFUNCTION(BlueprintCallable, Category = "Activate Lift")
	void SetLiftActivated(const bool ActivatedIn) { bIsActivated = ActivatedIn; }
	
	void OpenDoors(bool OpenLowerDoorsIn, class ASSJ_GameCharacter* CharacterThatActivated);

	bool GetTimelineIsPlaying() const { return bTimelineIsPlaying; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnUpperOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnUpperOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnLowerOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLowerOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void TimelineFloatReturn(float Value);

	UFUNCTION()
	void TimelineFinishedFunction();

	/** Declare a delegate to call with TimeLineFloatReturn */
	FOnTimelineFloat InterpFunction{};


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UCurveFloat* FCurve;

	// Time taken for curve to complete in seconds
	float MaxCurveTimeRange;

	void GetDoorOpeningCurve();

	bool bOpenLowerDoors;

	FVector Door1StartLocation;
	FVector Door1EndLocation;

	FVector Door2StartLocation;
	FVector Door2EndLocation;

	void SetDoorLocations();

	int32 CurrentDoorState;

	bool bTimelineIsPlaying;

	UPROPERTY()
	class ASSJ_GameCharacter* CharacterUsingThis;
};
