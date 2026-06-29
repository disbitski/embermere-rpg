#include "Characters/EmbermereCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTargetingComponent.h"
#include "Components/InputComponent.h"
#include "Data/EmbermereRulesData.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AEmbermereCharacter::AEmbermereCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Stats = CreateDefaultSubobject<UEmbermereStatsComponent>(TEXT("Stats"));
	Combat = CreateDefaultSubobject<UEmbermereCombatComponent>(TEXT("Combat"));
	Targeting = CreateDefaultSubobject<UEmbermereTargetingComponent>(TEXT("Targeting"));
	Hotbar = CreateDefaultSubobject<UEmbermereHotbarComponent>(TEXT("Hotbar"));
	Inventory = CreateDefaultSubobject<UEmbermereInventoryComponent>(TEXT("Inventory"));
	QuestLog = CreateDefaultSubobject<UEmbermereQuestLogComponent>(TEXT("QuestLog"));

	CharacterName = FText::FromString(TEXT("Embermere Adventurer"));
}

void AEmbermereCharacter::BeginPlay()
{
	Super::BeginPlay();
	PrimeStarterHotbar();
}

void AEmbermereCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AEmbermereCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEmbermereCharacter::MoveRight);
}

void AEmbermereCharacter::ApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass)
{
	UEmbermereRulesData* EffectiveRules = RulesData.Get() ? RulesData.Get() : NewObject<UEmbermereRulesData>(this);
	if (!EffectiveRules || !EffectiveRules->IsClassAllowed(NewRace, NewClass))
	{
		return;
	}

	Race = NewRace;
	Class = NewClass;
	PrimeStarterHotbar();
}

void AEmbermereCharacter::MoveForward(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		if (AEmbermerePlayerController* EmbermereController = Cast<AEmbermerePlayerController>(Controller))
		{
			EmbermereController->NotifyManualMoveForwardInput(Value);
		}

		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AEmbermereCharacter::MoveRight(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

bool AEmbermereCharacter::IsAlive_Implementation() const
{
	return Stats && !Stats->IsDead();
}

bool AEmbermereCharacter::IsHostileTo_Implementation(const AActor* Viewer) const
{
	return false;
}

FText AEmbermereCharacter::GetTargetDisplayName_Implementation() const
{
	return CharacterName;
}

void AEmbermereCharacter::SetTargetedByPlayer_Implementation(bool bIsTargeted)
{
	HandleTargetedByPlayer(bIsTargeted);
}

void AEmbermereCharacter::HandleTargetedByPlayer(bool bIsTargeted)
{
}

void AEmbermereCharacter::PrimeStarterHotbar()
{
	if (!Hotbar)
	{
		return;
	}

	UEmbermereRulesData* EffectiveRules = RulesData.Get() ? RulesData.Get() : NewObject<UEmbermereRulesData>(this);
	if (!EffectiveRules)
	{
		return;
	}

	FEmbermereClassDefinition ClassDefinition;
	if (!EffectiveRules->GetClassDefinition(Class, ClassDefinition))
	{
		return;
	}

	for (int32 Index = 0; Index < ClassDefinition.StarterAbilityIds.Num() && Index < 4; ++Index)
	{
		FEmbermereAbilityDefinition AbilityDefinition;
		if (EffectiveRules->GetAbilityDefinition(ClassDefinition.StarterAbilityIds[Index], AbilityDefinition))
		{
			Hotbar->SetAbilityInSlot(Index, AbilityDefinition);
		}
	}
}
