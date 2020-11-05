#include "Anims/Notifies/Hook/HookVisibility.h"

#include "../SekiroHookPJCharacter.h"

void UHookVisibility::Notify(USkeletalMeshComponent* MeshComp, 
                             UAnimSequenceBase* Animation) 
{
	if (MeshComp->IsValidLowLevel()) 
  {
		ASekiroHookPJCharacter *character = Cast<ASekiroHookPJCharacter>(MeshComp->GetOwner());

		if (character->IsValidLowLevel()) 
    {
			if (character->CheckHookVisibility())
				character->ChangeHookVisibility(false);
			else
				character->ChangeHookVisibility(true);
		}
	}
}
