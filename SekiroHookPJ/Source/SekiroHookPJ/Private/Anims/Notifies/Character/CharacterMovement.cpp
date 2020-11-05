#include "Anims/Notifies/Character/CharacterMovement.h"

#include "Components/CapsuleComponent.h"

#include "../SekiroHookPJCharacter.h"

UCharacterMovement::UCharacterMovement(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UCharacterMovement::NotifyBegin(USkeletalMeshComponent * MeshComp, 
                                     UAnimSequenceBase * Animation, 
                                     float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (character_->IsValidLowLevel()) 
  {
		start_loc_ = character_->CharacterTransform().GetLocation();
		end_loc_ = character_->LastFocusedPointLandLoc();
	}
}

void UCharacterMovement::NotifyTick(USkeletalMeshComponent * MeshComp, 
                                    UAnimSequenceBase * Animation, 
                                    float FrameDeltaTime) 
{
	if (character_->IsValidLowLevel())
		Location(FrameDeltaTime);
}

void UCharacterMovement::Location(float FrameDeltaTime) 
{
	if (current_movement_time_ < notify_total_time_) 
  {
		character_->LerpCharacterMovement(start_loc_, end_loc_, 
                                      current_movement_time_);
		current_movement_time_ += FrameDeltaTime;
	}
}



