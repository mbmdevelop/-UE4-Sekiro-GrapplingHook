#include "Anims/Notifies/Hook/Kunai/KunaiThrow.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

#include "../SekiroHookPJCharacter.h"

UKunaiThrow::UKunaiThrow(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UKunaiThrow::NotifyBegin(USkeletalMeshComponent *MeshComp, 
                              UAnimSequenceBase *Animation, 
                              float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (character_->IsValidLowLevel()) 
  {
		start_loc_ = character_->KunaiTransform().GetLocation();
		end_loc_ = character_->LastFocusedPointTrackerLoc();

		start_rot_ = character_->KunaiTransform().Rotator();
		end_rot_ = UKismetMathLibrary::FindLookAtRotation(start_loc_, end_loc_);
	}
}

void UKunaiThrow::NotifyTick(USkeletalMeshComponent *MeshComp, 
                             UAnimSequenceBase *Animation, 
                             float FrameDeltaTime) 
{
	if (character_->IsValidLowLevel()) 
  {
		Rotation(FrameDeltaTime);
		Throw(FrameDeltaTime);
	}
}

void UKunaiThrow::Rotation(float FrameDeltaTime) 
{
	float alpha = (current_rotation_time_ / notify_total_time_);

	if (alpha < 1.0f) 
  {
		character_->LerpKunaiRotation(start_rot_, end_rot_, alpha);
		current_rotation_time_ += FrameDeltaTime * 2.0f;
	}
}

void UKunaiThrow::Throw(float FrameDeltaTime) 
{
	if (current_movement_time_ < notify_total_time_) 
  {
		float next_frame_alpha = current_movement_time_ + FrameDeltaTime;
		character_->KunaiThrow(start_loc_, end_loc_, current_movement_time_,
                           next_frame_alpha);
		current_movement_time_ += FrameDeltaTime;
	}
}