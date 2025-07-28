// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonShooterGameMode.h"
#include "FirstPersonShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "Kismet/GameplayStatics.h"
#include "MonsterCharacter.h"
#include "TimerManager.h"

AFirstPersonShooterGameMode::AFirstPersonShooterGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// 몬스터 클래스는 에디터에서 설정하거나 코드에서 지정 가능
	// 예를 들면: ConstructorHelpers::FClassFinder<AMonsterCharacter> MonsterBP(TEXT("/Game/Monsters/BP_MonsterCharacter"));
	// MonsterClass = MonsterBP.Class;
}

void AFirstPersonShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 첫 소환 및 타이머 등록
	SpawnMonstersAsync();

	// 타이머 시작: 3초마다 SpawnMonstersInCircle 호출, 반복(true)
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AFirstPersonShooterGameMode::SpawnMonstersAsync, SpawnInterval, true);
}

void AFirstPersonShooterGameMode::SpawnMonstersAsync()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn || !NormalMonsterClass || !EliteMonsterClass || !BossMonsterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("One of Monster Classes is null!"));
		return;
	}

	const FVector Center = PlayerPawn->GetActorLocation();
	const int32 TotalSpawn = NumNormal + NumElite + NumBoss;

	// 1. 몬스터 클래스 배열 생성
	TArray<TSubclassOf<AMonsterCharacter>> MonsterClassesToSpawn;
	MonsterClassesToSpawn.Reserve(TotalSpawn);

	for (int32 i = 0; i < NumNormal; ++i)
	{
		MonsterClassesToSpawn.Add(NormalMonsterClass);
	}
	for (int32 i = 0; i < NumElite; ++i)
	{
		MonsterClassesToSpawn.Add(EliteMonsterClass);
	}
	for (int32 i = 0; i < NumBoss; ++i)
	{
		MonsterClassesToSpawn.Add(BossMonsterClass);
	}

	// 2. 몬스터 배열 셔플
	for (int32 i = MonsterClassesToSpawn.Num() - 1; i > 0; --i)
	{
		int32 SwapIndex = FMath::RandRange(0, i);
		if (i != SwapIndex)
		{
			MonsterClassesToSpawn.Swap(i, SwapIndex);
		}
	}

	// 3. AsyncTask: 위치 계산 백그라운드
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Center, TotalSpawn, MonsterClassesToSpawn]()
		{
			TArray<FVector> SpawnLocations;
			SpawnLocations.Reserve(TotalSpawn);

			for (int32 i = 0; i < TotalSpawn; ++i)
			{
				float AngleDeg = (360.f / TotalSpawn) * i;
				float AngleRad = FMath::DegreesToRadians(AngleDeg);

				FVector SpawnLocation = Center;
				SpawnLocation.X += SpawnRadius * FMath::Cos(AngleRad);
				SpawnLocation.Y += SpawnRadius * FMath::Sin(AngleRad);
				SpawnLocations.Add(SpawnLocation);
			}

			// 4. 메인 스레드에서 몬스터 소환
			AsyncTask(ENamedThreads::GameThread, [this, SpawnLocations, MonsterClassesToSpawn]()
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					for (int32 i = 0; i < MonsterClassesToSpawn.Num(); ++i)
					{
						TSubclassOf<AMonsterCharacter> MonsterClass = MonsterClassesToSpawn[i];
						FVector SpawnLoc = SpawnLocations[i];

						GetWorld()->SpawnActor<AMonsterCharacter>(MonsterClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
					}
				});
		});
}
