#include "Characters/EmbermerePracticeTargetActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEmbermerePracticeTargetActor::AEmbermerePracticeTargetActor()
{
	EnemyName = FText::FromString(TEXT("Fenwatch Practice Target"));
	RespawnDelaySeconds = 3.0f;

	bLootEnabled = false;
	bGrantsDefeatCredit = false;
	bPrototypeAiEnabled = false;
	bGameplayCollisionEnabled = false;
	bTraceTargetRingSurface = false;

	LootItem.Reset();
	LootQuantity = 0;
	LootDropChance = 0.0f;
	AggroRadius = 0.0f;
	AttackRange = 0.0f;
	AttackDamage = 0.0f;
	MoveSpeedCmPerSecond = 0.0f;
	LeashRadius = 0.0f;
	ReturnHomeSpeedCmPerSecond = 0.0f;

	NameplateHeight = 285.0f;
	TargetMarkerHeight = 335.0f;
	TargetRingRadius = 150.0f;
	TargetRingBoundsPadding = 0.0f;
	TargetRingHeightOffset = 16.0f;
	TargetRingSurfaceClearance = 16.0f;

	Tags.Remove(FName(TEXT("Hostile")));
	Tags.AddUnique(FName(TEXT("PracticeTarget")));

	if (Stats)
	{
		Stats->MaxHealth = 150.0f;
		Stats->CurrentHealth = 150.0f;
		Stats->MaxMana = 0.0f;
		Stats->CurrentMana = 0.0f;
	}

	SetActorEnableCollision(false);
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetGenerateOverlapEvents(false);
		Capsule->SetCanEverAffectNavigation(false);
	}
	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterMesh->SetGenerateOverlapEvents(false);
		CharacterMesh->SetCanEverAffectNavigation(false);
		CharacterMesh->SetVisibility(false);
		CharacterMesh->SetHiddenInGame(true);
	}
	FreezeAtAuthoredTransform();
}

void AEmbermerePracticeTargetActor::BeginPlay()
{
	Super::BeginPlay();
	FreezeAtAuthoredTransform();
}

void AEmbermerePracticeTargetActor::ResetPracticeTarget()
{
	const bool bWasSelected = IsSelectedByPlayer();
	FreezeAtAuthoredTransform();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(false);
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetGenerateOverlapEvents(false);
	}
	if (Stats)
	{
		Stats->InitializeVitals();
	}
	SetTargetedByPlayer_Implementation(bWasSelected);
	RefreshStatusEffectVfx();
}

void AEmbermerePracticeTargetActor::FreezeAtAuthoredTransform()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->GravityScale = 0.0f;
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
}
