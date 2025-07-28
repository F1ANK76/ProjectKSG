// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHPWidget.generated.h"

class UProgressBar;

UCLASS()
class FIRSTPERSONSHOOTER_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// ü�� ���� ���� �Լ� (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable)
	void SetHPRatio(float Ratio);

protected:
	// ���� ���ε��� ����
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
};
