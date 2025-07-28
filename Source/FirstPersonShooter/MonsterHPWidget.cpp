// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterHPWidget.h"
#include "Components/ProgressBar.h"

void UMonsterHPWidget::SetHPRatio(float Ratio)
{
	if (HPBar)
	{
		HPBar->SetPercent(FMath::Clamp(Ratio, 0.0f, 1.0f));
	}
}
