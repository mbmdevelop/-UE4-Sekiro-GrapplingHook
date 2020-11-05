#include "Anims/Notifies/Hook/Kunai/KunaiSwing.h"

#include "../SekiroHookPJCharacter.h"

UKunaiSwing::UKunaiSwing(const FObjectInitializer &ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UKunaiSwing::NotifyBegin(USkeletalMeshComponent *MeshComp, 
                              UAnimSequenceBase *Animation, 
                              float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UKunaiSwing::NotifyTick(USkeletalMeshComponent *MeshComp, 
                             UAnimSequenceBase *Animation, 
                             float FrameDeltaTime) 
{
	if (character_->IsValidLowLevel())
		Transform(FrameDeltaTime);
}

void UKunaiSwing::Transform(float FrameDeltaTime) 
{
	if (current_movement_time_ < notify_total_time_) 
  {
		character_->LerpKunaiTransform(character_->KunaiIdleSocketTransform(),
		character_->KunaiSwingSocketTransform(), current_movement_time_);
		current_movement_time_ += FrameDeltaTime;
	}
}




