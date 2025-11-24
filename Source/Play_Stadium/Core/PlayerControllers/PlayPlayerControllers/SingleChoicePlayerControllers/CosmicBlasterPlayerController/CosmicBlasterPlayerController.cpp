#include "CosmicBlasterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ACosmicBlasterPlayerController::OnPossess(APawn* InPawn)
{
        Super::OnPossess(InPawn);

        const ULocalPlayer* LocalPlayer = GetLocalPlayer();
        if (!LocalPlayer)
        {
                return;
        }

        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
                Subsystem->ClearAllMappings();

                if (InputMappingContext)
                {
                        Subsystem->AddMappingContext(InputMappingContext, 0);
                }
        }
}
