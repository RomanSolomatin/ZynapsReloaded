// Copyright (c) 2017 Bytecode Bits

#include "ZynapsReloaded.h"
#include "ZynapsPlayerState.h"
#include "UnrealNetwork.h"

// Log category
DEFINE_LOG_CATEGORY(LogZynapsPlayerState);

// Default constructor
AZynapsPlayerState::AZynapsPlayerState() : Super()
{
	SetCurrentState(EPlayerState::Playing);
	ResetGameScore();
	ResetLives();
	ResetSpeedUpLevel();
	LaserPower = 0;
	PlasmaBombs = false;
	HomingMissiles = false;
	SeekerMissiles = false;
	PowerUp = EPowerUp::SpeedUp;
	PowerUpActivationMode = false;
}

// Returns the set of replicated properties
void AZynapsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZynapsPlayerState, CurrentState);
	DOREPLIFETIME(AZynapsPlayerState, LaserPower);
	DOREPLIFETIME(AZynapsPlayerState, PlasmaBombs);
	DOREPLIFETIME(AZynapsPlayerState, HomingMissiles);
	DOREPLIFETIME(AZynapsPlayerState, SeekerMissiles);
}

// Returns the current player state
EPlayerState AZynapsPlayerState::GetCurrentState() const
{
	return CurrentState;
}

// Sets the current player state
void AZynapsPlayerState::SetCurrentState(EPlayerState State)
{
	// Change to the new state only if it is different from the current one
	if (CurrentState != State)
	{
		// Handle the new state
		switch (State)
		{
		case EPlayerState::Playing:
			UE_LOG(LogZynapsPlayerState, Verbose, TEXT("Setting new player state: Playing"));
			CurrentState = State;
			break;
		case EPlayerState::Destroyed:
			UE_LOG(LogZynapsPlayerState, Verbose, TEXT("Setting new player state: Destroyed"));
			CurrentState = State;
			ReduceLives();
			break;
		default:
			// Do nothing here
			UE_LOG(LogZynapsPlayerState, Warning, 
				TEXT("Tried to set and invalid player state. The state will not be changed"));
			break;
		}
	}
}

// Returns the power-up activation mode
bool AZynapsPlayerState::GetPowerUpActivationMode() const
{
	return PowerUpActivationMode;
}

// Sets the power-up activation mode
void AZynapsPlayerState::SetPowerUpActivationMode(bool NewPowerUpActivationMode)
{
	PowerUpActivationMode = NewPowerUpActivationMode;
}

// Returns the selected of the power-up
EPowerUp AZynapsPlayerState::GetSelectedPowerUp() const
{
	return PowerUp;
}

// Called when a fuel capsule is collected
void AZynapsPlayerState::FuelCapsuleCollected()
{
	if (PowerUpActivationMode)
	{
		ActivateSelectedPowerUp();
	}
	else
	{
		ShiftSelectedPowerUp();
	}
}

// Returns the game score
int32 AZynapsPlayerState::GetGameScore() const
{
	return GameScore;
}

// Increases the game score
void AZynapsPlayerState::IncreaseGameScore(int32 Points)
{
	GameScore += Points;
}

// Resets the game score
void AZynapsPlayerState::ResetGameScore()
{
	GameScore = 0;
}

// Returns the number of lives available
uint8 AZynapsPlayerState::GetLives() const
{
	return Lives;
}

// Increases a live
void AZynapsPlayerState::IncreaseLives()
{
	Lives++;
}

// Reduces a live and resets the power-up states
void AZynapsPlayerState::ReduceLives()
{
	Lives--;
	ResetSpeedUpLevel();
	LaserPower = 0;
	PlasmaBombs = false;
	HomingMissiles = false;
	SeekerMissiles = false;
	PowerUp = EPowerUp::SpeedUp;
	PowerUpActivationMode = false;
}

// Resets the number of lives
void AZynapsPlayerState::ResetLives()
{
	Lives = InitialLives;
}

// Returns the speed-up level
uint8 AZynapsPlayerState::GetSpeedUpLevel() const
{
	return SpeedUpLevel;
}

// Increases the speed-up level
void AZynapsPlayerState::IncreaseSpeedUpLevel()
{
	if (++SpeedUpLevel > 4)
	{
		SpeedUpLevel = 4;
	}
}

// Resets the speed-up level
void AZynapsPlayerState::ResetSpeedUpLevel()
{
	SpeedUpLevel = 0;
}

// Cycles through the power-ups
void AZynapsPlayerState::ShiftSelectedPowerUp()
{
	uint8 PowerUpIndex = (uint8)PowerUp;
	if (++PowerUpIndex > (uint8)EPowerUp::SeekerMissiles)
	{
		PowerUp = EPowerUp::SpeedUp;
	}
	else
	{
		PowerUp = (EPowerUp)PowerUpIndex;
	}
}

// Activates the selected power-up
void AZynapsPlayerState::ActivateSelectedPowerUp()
{
	// Activate the selected power-up
	switch (PowerUp)
	{
	case EPowerUp::SpeedUp:
		IncreaseSpeedUpLevel();
		break;
	case EPowerUp::LaserPower:
		IncreaseLaserPower();
		break;
	case EPowerUp::PlasmaBombs:
		PlasmaBombs = true;
		break;
	case EPowerUp::HomingMissiles:
		HomingMissiles = true;
		break;
	case EPowerUp::SeekerMissiles:
		SeekerMissiles = true;
		break;
	}

	// Reset the power-up selection
	PowerUp = EPowerUp::SpeedUp;
}

// Increases the laser power
void AZynapsPlayerState::IncreaseLaserPower()
{
	if (++LaserPower > 4)
	{
		LaserPower = 4;
	}
}
