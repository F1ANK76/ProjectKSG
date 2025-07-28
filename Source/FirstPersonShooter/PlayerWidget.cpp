// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include "FirstPersonShooterCharacter.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHealthBar();
}

void UPlayerWidget::SetOwnerCharacter(AFirstPersonShooterCharacter* InCharacter)
{
	OwnerCharacter = InCharacter;
	UpdateHealthBar();
}

void UPlayerWidget::UpdateHealthBar()
{
	if (OwnerCharacter.IsValid() && HealthBar)
	{
		float Ratio = OwnerCharacter->GetHealth() / OwnerCharacter->GetMaxHealth();
		HealthBar->SetPercent(Ratio);
	}
}

void UPlayerWidget::UpdateScore(int32 Score)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(Score));
	}
}

void UPlayerWidget::UpdateTime(float TimeLeft)
{
	if (TimeText)
	{
		TimeText->SetText(FText::AsNumber(FMath::FloorToInt(TimeLeft)));
	}
}

void UPlayerWidget::UpdateLevel(int32 Level, float ExpRatio)
{
	if (LevelBar)
	{
		LevelBar->SetPercent(ExpRatio);
	}

	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(Level));
	}
}

void UPlayerWidget::UpdateSkill(const FString& SkillKey)
{
	if (SkillKey == "Q")
	{
		if (Skill1Icon && Skill1Texture)
		{
			Skill1Icon->SetBrushFromTexture(Skill1Texture);
		}
	}
	else if (SkillKey == "E")
	{
		if (Skill2Icon && Skill2Texture)
		{
			Skill2Icon->SetBrushFromTexture(Skill2Texture);
		}
	}
}
