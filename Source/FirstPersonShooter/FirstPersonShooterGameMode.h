// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FirstPersonShooterGameMode.generated.h"

class AMonsterCharacter;

UCLASS(minimalapi)
class AFirstPersonShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFirstPersonShooterGameMode();

protected:
	virtual void BeginPlay() override;

private:
	// ���� ������ Ÿ�̸� �ڵ鷯
	FTimerHandle SpawnTimerHandle;

	// ���� Ŭ���� (�����ͳ� �ڵ忡�� ���� ����)
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMonsterCharacter> NormalMonsterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMonsterCharacter> EliteMonsterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMonsterCharacter> BossMonsterClass;

	int32 NumNormal = 3;
	int32 NumElite = 2;
	int32 NumBoss = 1;

	// ��Ƽ������ �̿�
	UFUNCTION()
	void SpawnMonstersAsync();

	// ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float SpawnRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	int32 SpawnCount = 7;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float SpawnInterval = 5.f;
};