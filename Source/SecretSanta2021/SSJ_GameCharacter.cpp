// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.


#include "SSJ_GameCharacter.h"
#include "AIController.h"
#include "SSJ_PickItem.h"
#include "SSJ_GameStateBase.h"
#include "SSJ_LevelInteractable.h"
#include "SSJ_LevelLift.h"
#include "SSJ_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASSJ_GameCharacter::ASSJ_GameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetRootComponent()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Comp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 1000.f;
	SpringArmComp->SetRelativeRotation(FRotator(-20.f, -90.f, 0.f));
	SpringArmComp->SetComponentTickEnabled(false);
	SpringArmComp->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	// Set so that camera does not turn when the player does
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = true;
	SpringArmComp->bInheritYaw = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->SetComponentTickEnabled(false);
	CameraComp->SetRelativeLocation(FVector(266.f, 163.f, 97.f));

	ItemDisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Display Mesh"));
	ItemDisplayMesh->SetupAttachment(GetMesh(), FName("PickupSocket"));

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->SetPlaneConstraintFromVectors(FVector(1, 0, 0), FVector(0, 0, 1));
	GetCharacterMovement()->bConstrainToPlane = true;

	AIControllerClass = ASSJ_AIController::StaticClass();
}

void ASSJ_GameCharacter::SetInteractingItem(ASSJ_LevelInteractable* InteractingItemIn, bool InteractingWithTrigger)
{
	CurrentInteractingItem = InteractingItemIn;
	bIsInteractingWithTrigger = InteractingWithTrigger;
}

void ASSJ_GameCharacter::SetPickupItem(ASSJ_PickItem* PickupItemIn)
{
	ItemToBePickedUp = PickupItemIn;
}

void ASSJ_GameCharacter::SetLiftBeingOverlapped(ASSJ_LevelLift* LevelLiftIn, bool IsLowerLevel)
{
	LiftBeingOverlapped = LevelLiftIn;
	bLowerLevelOverlapped = IsLowerLevel;
}

ASSJ_PickItem* ASSJ_GameCharacter::GetItemBeingHeld() const
{
	return ItemBeingHeld;
}

// Called when the game starts or when spawned
void ASSJ_GameCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Create and set a dynamic material from the existing material on the character */
	UMaterialInstanceDynamic* DynamicMaterial = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	/* Change color to the value set in ColourOfMesh */
	DynamicMaterial->SetVectorParameterValue("BodyColor", ColourOfMesh);

	GameStateBaseRef = Cast<ASSJ_GameStateBase>(GetWorld()->GetGameState());

	FTimerDelegate TimerDelegate;
	GetWorld()->GetTimerManager().SetTimer(GetControllerHandle, this, &ASSJ_GameCharacter::SetAIControllerRef, 0.5f, false);

}

void ASSJ_GameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController != AIControllerRef)
	{
		// Set the character rotation to the nearest yaw to avoid problems when re-possessing
		if (GetActorRotation().Yaw >= 0.f || GetActorRotation().Yaw <= -90.f)
		{
			SetActorRotation(FRotator(0.f, 90.f, 0.f));
		}
		else
		{
			SetActorRotation(FRotator(0.f, -90.f, 0.f));
		}
	}
}

void ASSJ_GameCharacter::UnPossessed()
{
	Super::UnPossessed();

	AIControllerRef->Possess(this);
	AIControllerRef->MoveToLocation(FVector(620.f, 70.f, 118.f), 10.f, false);
}

// Called every frame
void ASSJ_GameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// Called to bind functionality to input
void ASSJ_GameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction<FOnCharacterSwitch>("SwitchToCharacter1", IE_Pressed, this, &ASSJ_GameCharacter::SwitchCharacters, 0);
	PlayerInputComponent->BindAction<FOnCharacterSwitch>("SwitchToCharacter2", IE_Pressed, this, &ASSJ_GameCharacter::SwitchCharacters, 1);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASSJ_GameCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASSJ_GameCharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASSJ_GameCharacter::Interact);
	PlayerInputComponent->BindAction("PickupDropItem", IE_Pressed, this, &ASSJ_GameCharacter::PickupDropItem);
	
	PlayerInputComponent->BindAxis("MoveRight", this, &ASSJ_GameCharacter::MoveRight);
}


void ASSJ_GameCharacter::SwitchCharacters(int32 CharacterIndexToSwitchTo)
{
	if (GameStateBaseRef  && !GameStateBaseRef->GetIsDifferentCharacter(CharacterIndexToSwitchTo))
	{
		GameStateBaseRef->PossessNewCharacter(CharacterIndexToSwitchTo);
	}

}

void ASSJ_GameCharacter::MoveRight(float AxisVal)
{
	if (Controller && AxisVal != 0.f)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, AxisVal);
	}

	
}

void ASSJ_GameCharacter::Interact()
{
	// Only allow interaction if the character is currently overlapping an interaction item
	if (CurrentInteractingItem != nullptr && CurrentInteractingItem->GetNeededPickupItemGiven())
	{
		bIsInteractingWithTrigger ? CurrentInteractingItem->CallActivateTrigger() : CurrentInteractingItem->OnTriggeredActivated();
	}

	// Check if character is overlapping with a lift and if yes, use it
	if (LiftBeingOverlapped && !LiftBeingOverlapped->GetTimelineIsPlaying() && LiftBeingOverlapped->GetLiftIsActivated())
	{
		LiftBeingOverlapped->OpenDoors(bLowerLevelOverlapped, this);
	}
}

void ASSJ_GameCharacter::PickupDropItem()
{
	// Check if currently holding an item
	if (ItemToSpawn)
	{
		if (CurrentInteractingItem && ItemToSpawn == CurrentInteractingItem->GetNeededItemToInteract())
		{
			CurrentInteractingItem->SetTriggerStaticMesh(MeshToCreate);
		}
		else
		{
			// If not, drop the item
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector SpawnLoc = GetActorLocation();
			SpawnLoc.Z = GetActorLocation().Z + 80.f;
			const FRotator SpawnRot = FRotator(0.f);
		
			GetWorld()->SpawnActor<ASSJ_PickItem>(ItemToSpawn, SpawnLoc, SpawnRot, SpawnParameters);
		}

		ItemToSpawn = nullptr;
		ItemDisplayMesh->SetStaticMesh(nullptr);
	}
	
	// Check if PickupDropItem is valid and if it is, pickup the item, attach it to the socket
	if (ItemToBePickedUp != nullptr)
	{
		ItemBeingHeld = ItemToBePickedUp;
		ItemToSpawn = ItemToBePickedUp->GetClass();

		MeshToCreate = ItemToSpawn.GetDefaultObject()->DisplayMesh->GetStaticMesh();
		ItemDisplayMesh->SetStaticMesh(MeshToCreate);
				
		// Destroy the item in the world
		ItemToBePickedUp->DestroyThisItem();
		ItemToBePickedUp = nullptr;
	}
	
}

void ASSJ_GameCharacter::SetAIControllerRef()
{
	AIControllerRef = Cast<ASSJ_AIController>(UAIBlueprintHelperLibrary::GetAIController(this));
	
	if (!AIControllerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get Controller Ref on %s"), *GetName());
	}
}

