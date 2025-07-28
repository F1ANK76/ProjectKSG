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
	// 최고 점수 저장용
	UPROPERTY()
	int32 HighScore = 0;
};
