#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AnimationEnd.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UAnimationEnd : public UAnimNotify
{
 GENERATED_BODY()
 public:
  virtual void Notify(USkeletalMeshComponent* MeshComp, 
                      UAnimSequenceBase* Animation) override;
};
