// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.


#include "SSJ_LevelLift.h"
#include "SSJ_GameCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASSJ_LevelLift::ASSJ_LevelLift()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	UpperLevelCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Upper Level Collision"));
	UpperLevelCollision->SetupAttachment(RootComp);
	UpperLevelCollision->OnComponentBeginOverlap.AddDynamic(this, &ASSJ_LevelLift::OnUpperOverlapBegin);
	UpperLevelCollision->OnComponentEndOverlap.AddDynamic(this, &ASSJ_LevelLift::ASSJ_LevelLift::OnUpperOverlapEnd);

	UpperLevelDoor1Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Upper Door Mesh 1"));
	UpperLevelDoor1Mesh->SetupAttachment(UpperLevelCollision);
	UpperLevelDoor2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Upper Door Mesh 2"));
	UpperLevelDoor2Mesh->SetupAttachment(UpperLevelCollision);

	UpperLevelLightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Upper Level Light Mesh"));
	UpperLevelLightMesh->SetupAttachment(RootComp);
	LowerLevelLightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lower Level Light Mesh"));
	LowerLevelLightMesh->SetupAttachment(RootComp);
	
	UpperFloorTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("Upper Floor Target"));
	UpperFloorTarget->SetupAttachment(UpperLevelCollision);
	UpperLiftTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("Upper Lift Target"));
	UpperLiftTarget->SetupAttachment(UpperLevelCollision);

	UpperFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Upper Floor Mesh"));
	UpperFloorMesh->SetupAttachment(UpperLevelCollision);

	LowerLevelCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Lower Level Collision"));
	LowerLevelCollision->SetupAttachment(RootComp);
	LowerLevelCollision->OnComponentBeginOverlap.AddDynamic(this, &ASSJ_LevelLift::OnLowerOverlapBegin);
	LowerLevelCollision->OnComponentEndOverlap.AddDynamic(this, &ASSJ_LevelLift::OnUpperOverlapEnd);

	LowerLevelDoor1Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lower Door Mesh 1"));
	LowerLevelDoor1Mesh->SetupAttachment(LowerLevelCollision);
	LowerLevelDoor2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lower Door Mesh 2"));
	LowerLevelDoor2Mesh->SetupAttachment(LowerLevelCollision);

	LowerFloorTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("Lower Floor Target"));
	LowerFloorTarget->SetupAttachment(LowerLevelCollision);
	LowerLiftTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("Lower Lift Target"));
	LowerLiftTarget->SetupAttachment(LowerLevelCollision);

	LowerFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lower Floor Mesh"));
	LowerFloorMesh->SetupAttachment(LowerLevelCollision);

	DoorOpenTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFloatObject(TEXT("CurveFloat'/Game/Blueprints/LevelItems/DoorMovementCurve.DoorMovementCurve'"));
	FCurve = CurveFloatObject.Object;

	CurrentDoorState = 0;
	bTimelineIsPlaying = false;


}

void ASSJ_LevelLift::OpenDoors(bool OpenLowerDoorsIn, ASSJ_GameCharacter* CharacterThatActivated)
{
	bOpenLowerDoors = OpenLowerDoorsIn;
	CharacterUsingThis = CharacterThatActivated;
	
	if (FCurve && !bTimelineIsPlaying)
	{
		DoorOpenTimeline->PlayFromStart();
		bTimelineIsPlaying = true;
	}
}

// Called when the game starts or when spawned
void ASSJ_LevelLift::BeginPlay()
{
	Super::BeginPlay();

	GetDoorOpeningCurve();
	SetDoorLocations();

	UMaterialInstanceDynamic* DynamicMaterial = UpperLevelLightMesh->CreateAndSetMaterialInstanceDynamic(1);
	/* Change colour, depending on whether the lift is active or not */
	bIsActivated ? DynamicMaterial->SetVectorParameterValue("LightColor", FLinearColor(0.0f, 2.f, 0.f)) : DynamicMaterial->SetVectorParameterValue("LightColor", FLinearColor(2.0f, 0.f, 0.f));

	DynamicMaterial = LowerLevelLightMesh->CreateAndSetMaterialInstanceDynamic(1);
	/* Change colour, depending on whether the lift is active or not */
	bIsActivated ? DynamicMaterial->SetVectorParameterValue("LightColor", FLinearColor(0.0f, 2.f, 0.f)) : DynamicMaterial->SetVectorParameterValue("LightColor", FLinearColor(2.0f, 0.f, 0.f));
	
}

void ASSJ_LevelLift::OnUpperOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetLiftBeingOverlapped(this, false);
	}
}

void ASSJ_LevelLift::OnUpperOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetLiftBeingOverlapped(nullptr, false);
	}
}

void ASSJ_LevelLift::OnLowerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetLiftBeingOverlapped(this, true);
	}
}

void ASSJ_LevelLift::OnLowerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetLiftBeingOverlapped(nullptr, true);
	}
}

void ASSJ_LevelLift::TimelineFloatReturn(float Value)
{
	const FVector NewDoor1Location = FMath::Lerp(Door1StartLocation, Door1EndLocation, Value);
	const FVector NewDoor2Location = FMath::Lerp(Door2StartLocation, Door2EndLocation, Value);
	
	if (bOpenLowerDoors)
	{
		LowerLevelDoor1Mesh->SetRelativeLocation(NewDoor1Location);
		LowerLevelDoor2Mesh->SetRelativeLocation(NewDoor2Location);
	}
	else
	{
		UpperLevelDoor1Mesh->SetRelativeLocation(NewDoor1Location);
		UpperLevelDoor2Mesh->SetRelativeLocation(NewDoor2Location);
	}
}

void ASSJ_LevelLift::TimelineFinishedFunction()
{
	CurrentDoorState += 1;

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	/**LatentInfo.ExecutionFunction = FName("MoveToTargetFinished");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0; 
	*/
	if (CharacterUsingThis)
	{
		switch (CurrentDoorState)
		{
		case 1:
			// TODO Setup proper animations to make it look like the character is walking to their destinations
			//bOpenLowerDoors ? UKismetSystemLibrary::MoveComponentTo(CharacterUsingThis->GetRootComponent(), LowerLiftTarget->GetComponentLocation(), FRotator(0.f, 90.f, 0.f), true, true, 2.0f, false, EMoveComponentAction::Move, LatentInfo) : CharacterUsingThis->SetActorLocation(UpperLiftTarget->GetComponentLocation());
			//CharacterUsingThis->GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			
			bOpenLowerDoors ? CharacterUsingThis->SetActorLocation(LowerLiftTarget->GetComponentLocation()) : CharacterUsingThis->SetActorLocation(UpperLiftTarget->GetComponentLocation());
			DoorOpenTimeline->ReverseFromEnd();
			break;
		case 2:
			// Reverse the doors being opened
			//bOpenLowerDoors ? UKismetSystemLibrary::MoveComponentTo(CharacterUsingThis->GetRootComponent(), UpperLiftTarget->GetComponentLocation(), FRotator(0.f, 270.f, 0.f), true, true, 2.0f, false, EMoveComponentAction::Move, LatentInfo) : CharacterUsingThis->SetActorLocation(UpperLiftTarget->GetComponentLocation());
			bOpenLowerDoors ? CharacterUsingThis->SetActorLocation(UpperLiftTarget->GetComponentLocation()) : CharacterUsingThis->SetActorLocation(LowerLiftTarget->GetComponentLocation());
			bOpenLowerDoors ? bOpenLowerDoors = false : bOpenLowerDoors = true;
			DoorOpenTimeline->PlayFromStart();
			break;
		case 3:
			bOpenLowerDoors ? CharacterUsingThis->SetActorLocation(LowerFloorTarget->GetComponentLocation()) : CharacterUsingThis->SetActorLocation(UpperFloorTarget->GetComponentLocation());
			DoorOpenTimeline->ReverseFromEnd();
			break;
		case 4:
			CurrentDoorState = 0;
			bTimelineIsPlaying = false;
			CharacterUsingThis = nullptr;
			break;		
		default:
			break;
		}
	}

}

// Called every frame
void ASSJ_LevelLift::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(0, 0.f, FColor::Green, FString::FromInt(CurrentDoorState));
}

void ASSJ_LevelLift::GetDoorOpeningCurve()
{	if (FCurve)
{
	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	
	FOnTimelineEventStatic TimelineFinishedCallback;
	TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedFunction") });

	// Update function
	DoorOpenTimeline->AddInterpFloat(FCurve, InterpFunction, FName("Alpha"));

	// Function to call when the timeline has finished
	DoorOpenTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
	
	// Set the timeline settings
	DoorOpenTimeline->SetLooping(false);
	DoorOpenTimeline->SetIgnoreTimeDilation(true);
	
	float MinCurveTimeRange;
	FCurve->GetTimeRange(MinCurveTimeRange, MaxCurveTimeRange);
}
else
{
	UE_LOG(LogTemp, Warning, TEXT("Float Curve was not found.  Check the location in code"));
}
}

void ASSJ_LevelLift::SetDoorLocations()
{
	Door1StartLocation = UpperLevelDoor1Mesh->GetRelativeLocation();
	Door1EndLocation = FVector(Door1StartLocation.X + 100.f, Door1StartLocation.Y, Door1StartLocation.Z);

	Door2StartLocation = UpperLevelDoor2Mesh->GetRelativeLocation();
	Door2EndLocation = FVector(Door2StartLocation.X - 100.f, Door2StartLocation.Y, Door2StartLocation.Z);
}

