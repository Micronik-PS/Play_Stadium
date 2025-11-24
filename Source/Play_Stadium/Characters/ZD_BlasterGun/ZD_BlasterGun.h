#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "PaperZDCharacter.h"
#include "ZD_BlasterGun.generated.h"

class UInputAction;


UCLASS()
class PLAY_STADIUM_API AZD_BlasterGun : public APaperZDCharacter
{
	GENERATED_BODY()

	public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	private:
	void MoveHorizontal(const FInputActionValue& Value);

	private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

};
