// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.


#include "SSJ_LevelInteractable.h"
#include "SSJ_GameCharacter.h"
#include "Components/BoxComponent.h"
#include "SSJ_PickItem.h"

// Sets default values
ASSJ_LevelInteractable::ASSJ_LevelInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);
	
	TriggerCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Collision"));
	TriggerCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASSJ_LevelInteractable::OnTriggerOverlapBegin);
	TriggerCollisionComp->OnComponentEndOverlap.AddDynamic(this, &ASSJ_LevelInteractable::OnTriggerOverlapEnd);
	TriggerCollisionComp->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	TriggerCollisionComp->SetupAttachment(RootComponent);
	
	TriggerDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Mesh"));
	TriggerDisplayMesh->SetupAttachment(TriggerCollisionComp);

	TriggerItemPlacedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Placed Item Display Mesh"));
	TriggerItemPlacedMesh->SetupAttachment(TriggerDisplayMesh);

	TriggeredCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Triggered Collision"));
	TriggeredCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASSJ_LevelInteractable::OnTriggeredOverlapBegin);
	TriggeredCollisionComp->OnComponentEndOverlap.AddDynamic(this, &ASSJ_LevelInteractable::OnTriggeredOverlapEnd);
	TriggeredCollisionComp->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	TriggeredCollisionComp->SetupAttachment(RootComponent);
	
	TriggeredDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Triggered Mesh"));
	TriggeredDisplayMesh->SetupAttachment(TriggeredCollisionComp);

	TriggeredItemGivenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Triggered Item Display Mesh"));
	TriggeredItemGivenMesh->SetupAttachment(TriggeredDisplayMesh);

	bTriggerIsInteractedWith = false;
	TriggerActiveTime = 0.f;
	bTriggeredIsInteractedWith = false;
	bTriggeredIsNotReset = false;
	bNeededPickupItemGiven = false;
}

void ASSJ_LevelInteractable::SetTriggerStaticMesh(UStaticMesh* MeshIn)
{
	if (MeshIn != nullptr)
	{
		TriggerItemPlacedMesh->SetStaticMesh(MeshIn);
		bNeededPickupItemGiven = true;
	}
}

void ASSJ_LevelInteractable::CallActivateTrigger()
{
	OnTriggerActivated();

	if (TriggerActiveTime == 0.f)
	{
		TriggerCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
	else
	{
		// TODO Setup timer to reset trigger
	}
}

// Called when the game starts or when spawned
void ASSJ_LevelInteractable::BeginPlay()
{
	Super::BeginPlay();

	// If this gives out an item and has needs to show a display, set it
	if (PickupItemGiven && bGivenItemDisplayed)
	{
		TriggeredItemGivenMesh->SetStaticMesh(PickupItemGiven.GetDefaultObject()->DisplayMesh->GetStaticMesh());
	}

	PickupItemNeeded != nullptr ? bNeededPickupItemGiven = false : bNeededPickupItemGiven = true; 
}

void ASSJ_LevelInteractable::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		if (bTriggerIsInteractedWith)
		{
			GC->SetInteractingItem(this, true);
		}
	}
}

void ASSJ_LevelInteractable::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	bool bActorStillOverlapping = false;
	
	if (OverlappingActors.Num() > 0)
	{
		for (auto& It : OverlappingActors)
		{
			if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(It))
			{
				bActorStillOverlapping = true;
			}
		}
	}
	
	// Check if what we have overlapped is a player character
	if (!bActorStillOverlapping && bTriggerIsInteractedWith)
	{
		if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
		{
			GC->SetInteractingItem(nullptr, false);
		}
	}
}

void ASSJ_LevelInteractable::OnTriggeredOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		if (bTriggeredIsInteractedWith)
		{
			GC->SetInteractingItem(this, false);
		}
	}
}

void ASSJ_LevelInteractable::OnTriggeredOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		if (bTriggeredIsInteractedWith)
		{
			GC->SetInteractingItem(nullptr, false);
		}
	}
}

// Called every frame
void ASSJ_LevelInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

