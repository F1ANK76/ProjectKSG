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

	// �ݸ��� ���� ������Ʈ (��: ���� ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* CollisionSphere;

	// �ݸ��� ���� �Լ�
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

	// ü�¹� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	// ü�¹� UI Ŭ������ ������ �� �ְ�
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MonsterHPWidgetClass;
};
