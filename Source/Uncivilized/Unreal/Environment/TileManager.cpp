#include "TileManager.h"

ATileManager::ATileManager() {
	PrimaryActorTick.bCanEverTick = true;

	this->tileBiomes = new BiomeType[2048 * 2048]{};
	height = 2048;
	width = 2048;

	hexMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HexMeshInstances"));
	RootComponent = hexMeshInstances;
}

void ATileManager::BeginPlay() {
	Super::BeginPlay();
}

void ATileManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	delete[] this->tileBiomes;
}

void ATileManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
