#include "Anims/Notifies/Character/AnimationEnd.h"

#include "../SekiroHookPJCharacter.h"

void UAnimationEnd::Notify(USkeletalMeshComponent* MeshComp, 
                           UAnimSequenceBase* Animation) 
{
	if (MeshComp->IsValidLowLevel())
  {
		ASekiroHookPJCharacter *character = Cast<ASekiroHookPJCharacter>(MeshComp->GetOwner());
		
		if (character->IsValidLowLevel()) 
    {
			character->SetCharacterMovement(true);
			character->StopHookThrowing();
		}
	}
}
