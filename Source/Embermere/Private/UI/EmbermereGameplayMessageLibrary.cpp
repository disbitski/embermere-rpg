#include "UI/EmbermereGameplayMessageLibrary.h"
#include "Engine/Engine.h"
#include "Game/EmbermerePlayerController.h"

void UEmbermereGameplayMessageLibrary::PostGameplayMessage(
	const UObject* WorldContextObject,
	const FText& Message,
	FLinearColor MessageColor)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (World)
	{
		if (AEmbermerePlayerController* Controller = Cast<AEmbermerePlayerController>(World->GetFirstPlayerController()))
		{
			Controller->AddHudMessage(Message, MessageColor);
			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, MessageColor.ToFColor(true), Message.ToString());
	}
}
