#include "Anims/CharacterAnimInstance.h"

#include "Kismet/KismetMathLibrary.h"

UCharacterAnimInstance::UCharacterAnimInstance()
    : speed_(0.0f),is_in_air_(false) {}

void UCharacterAnimInstance::NativeBeginPlay() 
{
	Super::NativeBeginPlay();

	owning_actor_ = GetOwningActor();
	if (owning_actor_->IsValidLowLevel())
    character_ = Cast<ASekiroHookPJCharacter>(owning_actor_);
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (owning_actor_->IsValidLowLevel()) 
  {
		speed_ = owning_actor_->GetVelocity().Size();

		if(character_->IsValidLowLevel())
    {	
			is_in_air_ = character_->GetCharacterMovement()->IsFalling();
		}
	}
}

