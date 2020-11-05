#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HookeableActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

class AHookeablePoint;

UCLASS()
class SEKIROHOOKPJ_API AHookeableActor : public AActor
{
 GENERATED_BODY()
 public:	
  AHookeableActor();

	UPROPERTY(EditAnywhere, Category = "GamePlay")
	  bool can_walk_off_ledges_ ;

 private:	
	UPROPERTY(VisibleAnywhere, Category = "Components", 
            meta = (AllowPrivateAccess = "true"))
		USceneComponent* root_;

	UPROPERTY(VisibleAnywhere, Category = "Components", 
            meta = (AllowPrivateAccess = "true"))
		USceneComponent* mesh_;
};
