// FPSSaveGame.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FPSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONSHOOTER_API UFPSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// �ְ� ���� �����
	UPROPERTY()
	int32 HighScore = 0;
};
