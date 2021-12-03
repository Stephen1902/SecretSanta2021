// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.


#include "SSJ_PickItem.h"

#include "SSJ_GameCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
ASSJ_PickItem::ASSJ_PickItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Comp"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASSJ_PickItem::OnOverlapBegin);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ASSJ_PickItem::OnOverlapEnd);
	CollisionComp->SetSphereRadius(100.f);
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Display Mesh"));
	DisplayMesh->SetSimulatePhysics(true);
	DisplayMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	DisplayMesh->SetupAttachment(CollisionComp);
}

void ASSJ_PickItem::DestroyThisItem()
{
	this->Destroy();
}

// Called when the game starts or when spawned
void ASSJ_PickItem::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(FOnSpawnTimerHandle, this, &ASSJ_PickItem::TurnCollisionBackOn, 2.0f, false);
}

void ASSJ_PickItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ASSJ_PickItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetPickupItem(this);
	}
}

void ASSJ_PickItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End called"));
	// Check if what we have overlapped is a player character
	if (ASSJ_GameCharacter* GC = Cast<ASSJ_GameCharacter>(OtherActor))
	{
		GC->SetPickupItem(nullptr);
	}
}

void ASSJ_PickItem::TurnCollisionBackOn() const
{
	DisplayMesh->SetSimulatePhysics(false);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
}

