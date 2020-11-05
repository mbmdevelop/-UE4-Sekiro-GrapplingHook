#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "WalkablePlatforms.generated.h"

/**
 * 
 */
UCLASS()
class SEKIROHOOKPJ_API AWalkablePlatforms : public AStaticMeshActor
{
 GENERATED_BODY()

 public:
  UPROPERTY(EditAnywhere, Category = "GamePlay")
	  bool can_walk_off_ledges_;
};
