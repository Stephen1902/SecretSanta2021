// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.


#include "SSJ_GameStateBase.h"
#include "SSJ_GameCharacter.h"
#include "SSJ_PlayerController.h"
#include "Kismet/GameplayStatics.h"

void ASSJ_GameStateBase::PossessNewCharacter(const int32 CharacterIn)
{
	if (CharacterIn != CurrentCharacterSelected)
	{
		PC->Possess(Cast<ASSJ_GameCharacter>(CharacterToPossess[CharacterIn]));
		CurrentCharacterSelected = CharacterIn;
	}
}

void ASSJ_GameStateBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(GameStartTimer, this, &ASSJ_GameStateBase::StartDelay, 1.0f, false);
}

void ASSJ_GameStateBase::StartDelay()
{
	if (CharactersToSelect.Num() > 0)
	{
		PC = Cast<ASSJ_PlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
		{
			const TSubclassOf<ASSJ_GameCharacter> ActorsToCheckFor = ASSJ_GameCharacter::StaticClass();
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorsToCheckFor, CharacterToPossess);
			if (CharacterToPossess.Num() > 0)
			{
				CurrentCharacterSelected = 1;
				PossessNewCharacter(0);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No valid characters to possess found"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get player controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No characters in CharactersToSelect"));
	}

}
