// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class UProgressBar;
class AFirstPersonShooterCharacter;
class UImage;

UCLASS()
class FIRSTPERSONSHOOTER_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    void SetOwnerCharacter(AFirstPersonShooterCharacter* InCharacter);
    void UpdateHealthBar();
    void UpdateScore(int32 Score);
    void UpdateTime(float TimeLeft);
    void UpdateLevel(int32 Level, float ExpRatio);
    void UpdateSkill(const FString& SkillKey);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* LevelBar;

private:
    TWeakObjectPtr<AFirstPersonShooterCharacter> OwnerCharacter;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> ScoreText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> TimeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> LevelText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Skill1Icon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> Skill2Icon;

    // BP에서 설정 가능하게 이미지 텍스처 추가
    UPROPERTY(EditAnywhere, Category = "Skill Icon")
    TObjectPtr<UTexture2D> Skill1Texture;

    UPROPERTY(EditAnywhere, Category = "Skill Icon")
    TObjectPtr<UTexture2D> Skill2Texture;
};
