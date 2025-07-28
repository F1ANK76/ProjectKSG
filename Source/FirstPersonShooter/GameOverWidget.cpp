// GameOverWidget.cpp

#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "FPSSaveGame.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartButtonClicked);
	}
}

void UGameOverWidget::SetFinalScore(int32 Score)
{
	// 1. 최종 점수 UI 표시 (UI는 반드시 게임 스레드에서!)
	if (FinalScoreText)
	{
		FinalScoreText->SetText(FText::AsNumber(Score));
	}

	if (FinalRankText)
	{
		FString FinalTier = GetRankTier(Score);
		FinalRankText->SetText(FText::FromString(FinalTier));
	}

	// 2. 비동기로 SaveGame 로드 및 비교/저장
	Async(EAsyncExecution::ThreadPool, [this, Score]()
	{
		const FString SlotName = TEXT("HighScoreSlot");
		const int32 UserIndex = 0;

		// 저장된 최고 점수 불러오기
		UFPSSaveGame* LoadedGame = Cast<UFPSSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		int32 SavedHighScore = LoadedGame ? LoadedGame->HighScore : 0;

		// 새 최고 점수 판단
		const int32 NewHighScore = FMath::Max(SavedHighScore, Score);

		// 3. 저장 (현재 점수가 더 높다면)
		if (Score > SavedHighScore)
		{
			UFPSSaveGame* NewSaveGame = Cast<UFPSSaveGame>(UGameplayStatics::CreateSaveGameObject(UFPSSaveGame::StaticClass()));

			if (NewSaveGame)
			{
				NewSaveGame->HighScore = Score;
				UGameplayStatics::SaveGameToSlot(NewSaveGame, SlotName, UserIndex);
			}
		}

		// 4. UI는 다시 게임 스레드에서 표시
		AsyncTask(ENamedThreads::GameThread, [this, NewHighScore]()
		{
			if (HighScoreText)
			{
				HighScoreText->SetText(FText::AsNumber(NewHighScore));
			}

			if (HighRankText)
			{
				FString FinalTier = GetRankTier(NewHighScore);
				HighRankText->SetText(FText::FromString(FinalTier));
			}
		});
	});
}

void UGameOverWidget::OnRestartButtonClicked()
{
	// 입력 모드 원복
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	// 현재 레벨 다시 로드 (게임 리스타트)
	const FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, CurrentLevel);
}

FString UGameOverWidget::GetRankTier(int32 Score) const
{
	if (Score >= 2000) return TEXT("Challenger");
	else if (Score >= 1700) return TEXT("Grandmaster");
	else if (Score >= 1500) return TEXT("Master");
	else if (Score >= 1300) return TEXT("Diamond");
	else if (Score >= 1000) return TEXT("Platinum");
	else if (Score >= 700) return TEXT("Gold");
	else if (Score >= 500) return TEXT("Silver");
	else return TEXT("Bronze");
}
