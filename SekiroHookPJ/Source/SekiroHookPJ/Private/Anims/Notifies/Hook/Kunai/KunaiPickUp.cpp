#include "Anims/Notifies/Hook/Kunai/KunaiPickUp.h"

#include "../SekiroHookPJCharacter.h"

UKunaiPickUp::UKunaiPickUp(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UKunaiPickUp::NotifyBegin(USkeletalMeshComponent * MeshComp, 
                               UAnimSequenceBase * Animation, 
                               float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (character_->IsValidLowLevel()) 
  {
		start_loc_ = character_->KunaiTransform().GetLocation();
		end_loc_ = character_->KunaiIdleSocketTransform().GetLocation();
	}
}

void UKunaiPickUp::NotifyTick(USkeletalMeshComponent * MeshComp, 
                              UAnimSequenceBase * Animation, 
                              float FrameDeltaTime) 
{
	if (character_->IsValidLowLevel())
		PickUp(FrameDeltaTime);
}

void UKunaiPickUp::NotifyEnd(USkeletalMeshComponent * MeshComp, 
                             UAnimSequenceBase * Animation) 
{
	if (character_->IsValidLowLevel())
		character_->AttachKunai();
}

void UKunaiPickUp::PickUp(float FrameDeltaTime) 
{
	if (current_movement_time_ < notify_total_time_) 
  {
		character_->KunaiPickUp(start_loc_, 
                            character_->KunaiIdleSocketTransform().GetLocation(),
                            current_movement_time_);
		current_movement_time_ += FrameDeltaTime;
	}
}