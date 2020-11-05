#include "Anims/Notifies/Character/CharacterRotation.h"

#include "Kismet/KismetMathLibrary.h"

#include "../SekiroHookPJCharacter.h"

UCharacterRotation::UCharacterRotation(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UCharacterRotation::NotifyBegin(USkeletalMeshComponent * MeshComp, 
                                     UAnimSequenceBase * Animation, 
                                     float TotalDuration) 
{
	Super::NotifyBegin(MeshComp,Animation,TotalDuration);

	if (character_->IsValidLowLevel()) 
  {
		start_rot_ = character_->CharacterTransform().Rotator();
		end_rot_ = UKismetMathLibrary::FindLookAtRotation(
        character_->CharacterTransform().GetLocation(), 
        character_->LastFocusedPointLandLoc());
		
    // NOTE: Only need the Yaw
		end_rot_.Pitch = 0.0f;
		end_rot_.Roll = 0.0f;
	}
}

void UCharacterRotation::NotifyTick(USkeletalMeshComponent * MeshComp, 
                                    UAnimSequenceBase * Animation, 
                                    float FrameDeltaTime) 
{
	if (character_->IsValidLowLevel())
		Rotation(FrameDeltaTime);
}

void UCharacterRotation::Rotation(float FrameDeltaTime) {

	float alpha = (current_rotation_time_ / notify_total_time_);

	if (alpha < 1.0f) 
  {
		character_->LerpCharacterRotation(start_rot_, end_rot_, alpha);
		current_rotation_time_ += FrameDeltaTime;
	}
}


