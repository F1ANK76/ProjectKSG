// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

class UWidgetComponent;

UCLASS()
class FIRSTPERSONSHOOTER_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterCharacter();
	void ApplyDamage(float DamageAmount);
	void UpdateHPBar();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// 콜리전 영역 컴포넌트 (예: 구형 영역)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* CollisionSphere;

	// 콜리전 시작 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	ACharacter* TargetPlayer;

	float MoveSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 ScoreValue = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 ExpAmount = 10;

	// 체력바 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	// 체력바 UI 클래스를 설정할 수 있게
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MonsterHPWidgetClass;
};
