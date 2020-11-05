#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "HookVisibility.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UHookVisibility : public UAnimNotify
{
 GENERATED_BODY()
 public:
  virtual void Notify(USkeletalMeshComponent* MeshComp, 
                      UAnimSequenceBase* Animation) override;
};
