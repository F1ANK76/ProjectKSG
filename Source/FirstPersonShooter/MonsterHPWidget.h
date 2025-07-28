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
	// 체력 비율 설정 함수 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable)
	void SetHPRatio(float Ratio);

protected:
	// 위젯 바인딩용 변수
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
};
