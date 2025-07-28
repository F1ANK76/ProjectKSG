#include "MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "FirstPersonShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "MonsterHPWidget.h"
#include "Components/WidgetComponent.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(100.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMonsterCharacter::OnOverlapBegin);

	// ���� ������Ʈ ���� �� ����
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);

	// ���� ������Ʈ ����
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(100.f, 15.f));
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBarWidget->SetVisibility(true);
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾� ĳ���� ���� ����
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// ü�� �ʱ�ȭ
	CurrentHealth = MaxHealth;

	if (MonsterHPWidgetClass)
	{
		HealthBarWidget->SetWidgetClass(MonsterHPWidgetClass);
		HealthBarWidget->InitWidget();

		// �ʱ� ü�� �ݿ�
		UpdateHPBar();
	}
}

void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer)
	{
		FVector MyLocation = GetActorLocation();
		FVector PlayerLocation = TargetPlayer->GetActorLocation();

		// �÷��̾� ���� ���
		FVector Direction = (PlayerLocation - MyLocation).GetSafeNormal();

		// ��ġ �̵�
		FVector NewLocation = MyLocation + Direction * MoveSpeed * DeltaTime;
		SetActorLocation(NewLocation);

		// �ٶ󺸴� ���� ����
		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(NewRotation);
	}
}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterCharacter::ApplyDamage(float DamageAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	UpdateHPBar();

	if (CurrentHealth <= 0.f)
	{
		AFirstPersonShooterCharacter* Player = Cast<AFirstPersonShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		if (Player)
		{
			Player->AddScore(ScoreValue);
			Player->AddTimeBonus(1.f);
			Player->AddExperience(ExpAmount);
		}

		Destroy();
	}
}

void AMonsterCharacter::UpdateHPBar()
{
	if (UMonsterHPWidget* HPWidget = Cast<UMonsterHPWidget>(HealthBarWidget->GetUserWidgetObject()))
	{
		float HPRatio = CurrentHealth / MaxHealth;
		HPWidget->SetHPRatio(HPRatio);
	}
}

void AMonsterCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		AFirstPersonShooterCharacter* Player = Cast<AFirstPersonShooterCharacter>(OtherActor);

		if (Player)
		{
			Player->ApplyDamage(20.f); // �÷��̾� ApplyDamage �Լ��� �־�� ��
			Destroy();
		}
	}
}

