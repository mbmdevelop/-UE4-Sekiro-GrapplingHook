#include "Anims/Notifies/Hook/AllowPointFocus.h"

#include "../SekiroHookPJCharacter.h"


void UAllowPointFocus::Notify(USkeletalMeshComponent* MeshComp, 
                              UAnimSequenceBase* Animation) 
{
	if (MeshComp->IsValidLowLevel()) 
  {
		ASekiroHookPJCharacter *character = Cast<ASekiroHookPJCharacter>(MeshComp->GetOwner());

		if (character->IsValidLowLevel())
			character->SetFocusAllowed(true);
	}
}
