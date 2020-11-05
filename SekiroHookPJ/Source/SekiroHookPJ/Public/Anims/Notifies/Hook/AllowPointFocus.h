#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AllowPointFocus.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UAllowPointFocus : public UAnimNotify
{
 GENERATED_BODY()
 public:
  virtual void Notify(USkeletalMeshComponent* MeshComp, 
                      UAnimSequenceBase* Animation) override;
};
