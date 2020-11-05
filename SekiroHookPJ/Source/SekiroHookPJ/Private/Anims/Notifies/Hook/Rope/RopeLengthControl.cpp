#include "Anims/Notifies/Hook/Rope/RopeLengthControl.h"

#include "../SekiroHookPJCharacter.h"


URopeLengthControl::URopeLengthControl(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void URopeLengthControl::NotifyBegin(USkeletalMeshComponent *MeshComp, 
                                     UAnimSequenceBase *Animation, 
                                     float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void URopeLengthControl::NotifyTick(USkeletalMeshComponent *MeshComp, 
                                    UAnimSequenceBase *Animation, 
                                    float FrameDeltaTime) 
{
	if(character_->IsValidLowLevel())
		UpdateRopeLength(FrameDeltaTime);
}

void URopeLengthControl::UpdateRopeLength(float FrameDeltaTime) 
{
	if (current_movement_time_ < notify_total_time_) 
  {
		character_->UpdateRopeLength(current_movement_time_);
		current_movement_time_ += FrameDeltaTime;
	}
}
