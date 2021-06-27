#include "PlayerSpawn.h"


APlayerSpawn::APlayerSpawn()
{

	bIsTaken = false;

}

bool APlayerSpawn::IsTaken()
{
	return bIsTaken;
}

void APlayerSpawn::SetTaken(bool Taken)
{
	bIsTaken = Taken;
}

