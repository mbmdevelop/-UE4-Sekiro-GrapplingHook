#include "Anims/Notifies/Hook/Rope/TautenRope.h"

#include "../SekiroHookPJCharacter.h"

UTautenRope::UTautenRope(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UTautenRope::NotifyBegin(USkeletalMeshComponent *MeshComp, 
                              UAnimSequenceBase *Animation, 
                              float TotalDuration) 
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (character_->IsValidLowLevel()) 
		character_->SetRopeStiffness(true);
}

void UTautenRope::NotifyEnd(USkeletalMeshComponent *MeshComp, 
                            UAnimSequenceBase *Animation) 
{
	if (character_->IsValidLowLevel()) 
		character_->SetRopeStiffness(false);
}