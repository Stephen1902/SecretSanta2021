// Copyright 2021 DME Games.  Made for the 2021 Secret Santa Jam.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SSJ_GameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SECRETSANTA2021_API ASSJ_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	// A list of characters that the player can switch between
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Characters")
	TArray<TSubclassOf <class ASSJ_GameCharacter>> CharactersToSelect;

	void PossessNewCharacter(int32 CharacterIn);

	bool GetIsDifferentCharacter(const int32 CharacterIn) const { return CharacterIn == CurrentCharacterSelected; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AActor*> CharacterToPossess;

	UPROPERTY()
	class ASSJ_PlayerController* PC;

	int32 CurrentCharacterSelected;

	FTimerHandle GameStartTimer;

	void StartDelay();
};
