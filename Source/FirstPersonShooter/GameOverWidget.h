// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPERSONSHOOTER_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 점수 전달용 함수
	void SetFinalScore(int32 Score);

protected:
	// 리스타트 버튼 바인딩용 변수
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FinalScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HighScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FinalRankText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HighRankText;

	// 클릭 이벤트 함수
	UFUNCTION()
	void OnRestartButtonClicked();

	UFUNCTION()
	FString GetRankTier(int32 Score) const;
};
