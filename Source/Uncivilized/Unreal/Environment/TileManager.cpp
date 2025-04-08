#include "TileManager.h"

ATileManager::ATileManager() {
	PrimaryActorTick.bCanEverTick = true;

	this->tileBiomes = new BiomeType[2048 * 2048]{};
	gridHeight = 2048;
	gridWidth = 2048;

	hexMeshInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HexMeshInstances"));
	RootComponent = hexMeshInstances;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Meshes/Hex.fbx"));

	if (MeshFinder.Succeeded()) {
		hexMeshInstances->SetStaticMesh(MeshFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/Materials/HexMaterial.uasset"));
	if (MaterialFinder.Succeeded()) {
		baseMaterial = MaterialFinder.Object;
	}
}

void ATileManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	delete[] this->tileBiomes;
}

void ATileManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ATileManager::BeginPlay() {
	Super::BeginPlay();

	if (!hexMeshInstances || !baseMaterial)
		return;

	hexMeshInstances->NumCustomDataFloats = 1;
	hexMeshInstances->SetMaterial(0, baseMaterial);

	for (size_t i = 0; i < (2048 * 2048); i++) {
		BiomeType type = FMath::RandBool() ? BiomeType::GRASSLAND : BiomeType::CHAPPARAL
	}

	if (AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		setPlayerPosition(player->GetActorLocation());
	}
}
