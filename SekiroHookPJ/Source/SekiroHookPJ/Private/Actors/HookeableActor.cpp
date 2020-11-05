#include "Actors/HookeableActor.h"

#include "Actors/HookeablePoint.h"

AHookeableActor::AHookeableActor()
  : can_walk_off_ledges_(false)
{
  PrimaryActorTick.bCanEverTick = false;

  root_ = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

  RootComponent = root_;
  mesh_->SetupAttachment(RootComponent);
}



