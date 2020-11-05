#pragma once

#include "CoreMinimal.h"
#include "Anims/Notifies/LerpNotifyStateBaseClass.h"
#include "TautenRope.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UTautenRope : public ULerpNotifyStateBaseClass
{
 GENERATED_BODY()
 public:
	UTautenRope(const FObjectInitializer &ObjectInitializer);

	virtual void NotifyBegin(USkeletalMeshComponent *MeshComp, 
                           UAnimSequenceBase *Animation, 
                           float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent *MeshComp, 
                         UAnimSequenceBase *Animation) override;
};
