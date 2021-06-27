#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "PlayerSpawn.generated.h"

/**
 * 
 */
UCLASS()
class TST_API APlayerSpawn : public ATargetPoint
{
	GENERATED_BODY()
public:

	APlayerSpawn();
protected:

	bool bIsTaken;

public: 

	bool IsTaken();
	void SetTaken(bool Taken);
};
