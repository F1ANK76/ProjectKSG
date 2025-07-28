// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonShooterCharacter.h"
#include "FirstPersonShooterProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "PlayerWidget.h"
#include "GameOverWidget.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFirstPersonShooterCharacter

AFirstPersonShooterCharacter::AFirstPersonShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	UnlockedSkills.Add("Q", false);
	UnlockedSkills.Add("E", false);
	UnlockedSkills.Add("R", false);
}

void AFirstPersonShooterCharacter::ApplyDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if (PlayerWidget)
	{
		PlayerWidget->UpdateHealthBar();
	}

	if (CurrentHealth <= 0.f)
	{
		UGameplayStatics::SetGamePaused(this, true);

		if (GameOverWidgetClass)
		{
			UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetClass);

			if (GameOverWidget)
			{
				// 점수 전달
				GameOverWidget->SetFinalScore(CurrentScore);

				GameOverWidget->AddToViewport();

				APlayerController* PC = GetWorld()->GetFirstPlayerController();

				if (PC)
				{
					FInputModeUIOnly InputMode;
					PC->SetInputMode(InputMode);
					PC->bShowMouseCursor = true;
				}
			}
		}
	}
}

void AFirstPersonShooterCharacter::AddScore(int32 Amount)
{
	CurrentScore += Amount;

	if (PlayerWidget)  // UI 연결된 경우 갱신
	{
		PlayerWidget->UpdateScore(CurrentScore);
	}
}

void AFirstPersonShooterCharacter::AddTimeBonus(float BonusTime)
{
	TimeRemaining += BonusTime;

	// 남은 시간이 갱신되면 UI도 업데이트
	if (PlayerWidget)
	{
		PlayerWidget->UpdateTime(TimeRemaining);
	}
}

void AFirstPersonShooterCharacter::AddExperience(int32 Exp)
{
	CurrentExp += Exp;

	while (CurrentExp >= ExpToLevelUp)
	{
		CurrentExp -= ExpToLevelUp;
		CurrentLevel++;

		// 레벨업 시 스킬 해금 체크
		if (CurrentLevel >= 3 && UnlockedSkills.Contains("Q") && !UnlockedSkills["Q"])
		{
			UnlockedSkills["Q"] = true;

			if (PlayerWidget)
			{
				PlayerWidget->UpdateSkill("Q");
			}
		}

		if (CurrentLevel >= 5 && UnlockedSkills.Contains("E") && !UnlockedSkills["E"])
		{
			UnlockedSkills["E"] = true;

			if (PlayerWidget)
			{
				PlayerWidget->UpdateSkill("E");
			}
		}
	}

	if (PlayerWidget)
	{
		PlayerWidget->UpdateLevel(CurrentLevel, GetCurrentExpRatio());
	}
}

float AFirstPersonShooterCharacter::GetCurrentExpRatio() const
{
	return (float)CurrentExp / (float)ExpToLevelUp;
}

float AFirstPersonShooterCharacter::GetHealth() const
{
	return CurrentHealth;
}

float AFirstPersonShooterCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

//////////////////////////////////////////////////////////////////////////// Input

void AFirstPersonShooterCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFirstPersonShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::Look);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::Dash);
		EnhancedInputComponent->BindAction(HighJumpAction, ETriggerEvent::Triggered, this, &AFirstPersonShooterCharacter::HighJump);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFirstPersonShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport();
			PlayerWidget->SetOwnerCharacter(this); // 이 함수가 위젯 클래스에 있어야 함
			PlayerWidget->UpdateTime(TimeRemaining);
		}
	}

	// 1초 간격으로 카운트다운
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AFirstPersonShooterCharacter::UpdateCountdown, 1.0f, true);
}

void AFirstPersonShooterCharacter::UpdateCountdown()
{
	TimeRemaining -= 1.f;

	if (PlayerWidget)
	{
		PlayerWidget->UpdateTime(TimeRemaining);
	}

	if (TimeRemaining <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		// 시간 종료: 게임 오버 처리
		UGameplayStatics::SetGamePaused(this, true);

		if (GameOverWidgetClass)
		{
			UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetClass);

			if (GameOverWidget)
			{
				GameOverWidget->SetFinalScore(CurrentScore); // 점수 전달
				GameOverWidget->AddToViewport();

				APlayerController* PC = GetWorld()->GetFirstPlayerController();

				if (PC)
				{
					FInputModeUIOnly InputMode;
					PC->SetInputMode(InputMode);
					PC->bShowMouseCursor = true;
				}
			}
		}
	}
}

void AFirstPersonShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFirstPersonShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFirstPersonShooterCharacter::Dash()
{
	// Q 스킬 사용 가능 여부 체크
	if (!UnlockedSkills.Contains("Q") || !UnlockedSkills["Q"])
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Unlock Q"));
		return; // 스킬 잠겨있으면 함수 종료
	}

	const FVector DashDirection = GetActorForwardVector();
	const FVector DashVelocity = DashDirection * 5000;

	LaunchCharacter(DashVelocity, true, true);
}

void AFirstPersonShooterCharacter::HighJump()
{
	// Q 스킬 사용 가능 여부 체크
	if (!UnlockedSkills.Contains("E") || !UnlockedSkills["E"])
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Unlock E"));
		return; // 스킬 잠겨있으면 함수 종료
	}

	const float HighJumpPower = 1500.f;
	LaunchCharacter(FVector(0.f, 0.f, HighJumpPower), false, true);
}
