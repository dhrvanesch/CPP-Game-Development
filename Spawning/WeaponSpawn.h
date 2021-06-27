#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "WeaponSpawn.generated.h"
/**
 * 
 */
UCLASS()
class TST_API AWeaponSpawn : public ATargetPoint
{
	GENERATED_BODY()

public:
		AWeaponSpawn();


protected:

	bool bIsTaken;

public:

	bool IsTaken();
	void SetTaken(bool Taken);
};
