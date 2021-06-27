#include "WeaponSpawn.h"

AWeaponSpawn::AWeaponSpawn()
{

	bIsTaken = false;

}

bool AWeaponSpawn::IsTaken()
{
	return bIsTaken;
}

void AWeaponSpawn::SetTaken(bool Taken)
{
	bIsTaken = Taken;
}
