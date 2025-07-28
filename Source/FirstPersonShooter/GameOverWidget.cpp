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
	// 1. ���� ���� UI ǥ�� (UI�� �ݵ�� ���� �����忡��!)
	if (FinalScoreText)
	{
		FinalScoreText->SetText(FText::AsNumber(Score));
	}

	if (FinalRankText)
	{
		FString FinalTier = GetRankTier(Score);
		FinalRankText->SetText(FText::FromString(FinalTier));
	}

	// 2. �񵿱�� SaveGame �ε� �� ��/����
	Async(EAsyncExecution::ThreadPool, [this, Score]()
	{
		const FString SlotName = TEXT("HighScoreSlot");
		const int32 UserIndex = 0;

		// ����� �ְ� ���� �ҷ�����
		UFPSSaveGame* LoadedGame = Cast<UFPSSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		int32 SavedHighScore = LoadedGame ? LoadedGame->HighScore : 0;

		// �� �ְ� ���� �Ǵ�
		const int32 NewHighScore = FMath::Max(SavedHighScore, Score);

		// 3. ���� (���� ������ �� ���ٸ�)
		if (Score > SavedHighScore)
		{
			UFPSSaveGame* NewSaveGame = Cast<UFPSSaveGame>(UGameplayStatics::CreateSaveGameObject(UFPSSaveGame::StaticClass()));

			if (NewSaveGame)
			{
				NewSaveGame->HighScore = Score;
				UGameplayStatics::SaveGameToSlot(NewSaveGame, SlotName, UserIndex);
			}
		}

		// 4. UI�� �ٽ� ���� �����忡�� ǥ��
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
	// �Է� ��� ����
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	// ���� ���� �ٽ� �ε� (���� ����ŸƮ)
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
