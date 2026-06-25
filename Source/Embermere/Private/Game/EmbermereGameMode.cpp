#include "Game/EmbermereGameMode.h"
#include "Characters/EmbermereCharacter.h"
#include "Game/EmbermerePlayerController.h"

AEmbermereGameMode::AEmbermereGameMode()
{
	DefaultPawnClass = AEmbermereCharacter::StaticClass();
	PlayerControllerClass = AEmbermerePlayerController::StaticClass();
}
