// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FirstPersonShooterCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFirstPersonShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// Input Action (Q키 대시)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* HighJumpAction;
	
public:
	AFirstPersonShooterCharacter();

	void ApplyDamage(float Damage);
	void AddScore(int32 Amount);
	void AddTimeBonus(float BonusTime);

	void AddExperience(int32 Exp);

	int32 GetCurrentLevel() const { return CurrentLevel; }
	float GetCurrentExpRatio() const; // 경험치 비율 (0.0 ~ 1.0)

	float GetHealth() const;
	float GetMaxHealth() const;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dash();
	void HighJump();

	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void BeginPlay() override;

private:
	float CurrentHealth = 100.f;
	float MaxHealth = 100.f;

	int32 CurrentScore = 0;

	float TimeRemaining = 10.f;

	int32 CurrentExp = 0;
	int32 CurrentLevel = 1;
	int32 ExpToLevelUp = 100; // 다음 레벨까지 필요한 경험치

	FTimerHandle CountdownTimerHandle;

	TMap<FName, bool> UnlockedSkills;

	void UpdateCountdown();

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UPlayerWidget> PlayerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameOverWidget> GameOverWidgetClass;

	UPROPERTY()
	class UPlayerWidget* PlayerWidget;
};

