#include "Anims/Notifies/LerpNotifyStateBaseClass.h"

#include "../SekiroHookPJCharacter.h"

ULerpNotifyStateBaseClass::ULerpNotifyStateBaseClass(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer),
      character_(nullptr),
      notify_total_time_(0.0f),
      start_loc_(FVector::ZeroVector),
      end_loc_(FVector::ZeroVector),
      current_movement_time_(0.0f),
      start_rot_(FRotator::ZeroRotator),
      end_rot_(FRotator::ZeroRotator),
      current_rotation_time_(0.0f) {}

void ULerpNotifyStateBaseClass::NotifyBegin(USkeletalMeshComponent * MeshComp, 
                                            UAnimSequenceBase * Animation, 
                                            float TotalDuration) 
{
	if (MeshComp->GetOwner()->IsValidLowLevel()) 
  {
		character_ = Cast<ASekiroHookPJCharacter>(MeshComp->GetOwner());

		notify_total_time_ = TotalDuration;
		current_movement_time_ = 0.0f;
		current_rotation_time_ = 0.0f;
	}
}



