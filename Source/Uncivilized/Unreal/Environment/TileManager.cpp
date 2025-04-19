#include "TileManager.h"
#include <Kismet/GameplayStatics.h>

ATileManager::ATileManager() {
	PrimaryActorTick.bCanEverTick = true;
	gridHeight = 2048;
	gridWidth = 2048;
	numberOfTrees = 30;
	this->tileBiomes = new BiomeType[gridHeight * gridWidth]{};
	this->occupancyMasks = new BitMatrix16x16[gridHeight * gridWidth]{};
}

void ATileManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	delete[] this->tileBiomes;
	delete[] this->occupancyMasks;
}

void ATileManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		setPlayerPosition(player->GetActorLocation());
	}
}

void ATileManager::setPlayerPosition(const FVector& newPosition) {
	FIntPoint newChunkPos = worldToChunkPosition(newPosition);
	if (newChunkPos != playerChunkPosition) {
		playerChunkPosition = newChunkPos;
		updateVisibleChunks(newPosition);
	}
}

void ATileManager::updateVisibleChunks(const FVector& playerPosition) {

	TSet<FIntPoint> chunksToBeVisible;
	int32 checkDistance = FMath::CeilToInt(renderDistance);
	for (int32 dx = -checkDistance; dx <= checkDistance; ++dx) {
		for (int32 dy = -checkDistance; dy <= checkDistance; ++dy) {
			FIntPoint checkChunkPos(playerChunkPosition.X + dx, playerChunkPosition.Y + dy);
			if (isChunkInRenderDistance(checkChunkPos, playerChunkPosition)) {
				chunksToBeVisible.Add(checkChunkPos);
			}
		}
	}

	TArray<FIntPoint> chunksToUnload;
	for (auto& pair : loadedTileChunks) {
		if (!chunksToBeVisible.Contains(pair.first)) {
			chunksToUnload.Add(pair.first);
		}
	}
	for (const FIntPoint& chunkPos : chunksToUnload) {
		unloadChunk(chunkPos);
	}

	for (const FIntPoint& chunkPos : chunksToBeVisible) {
		if (loadedTileChunks.find(chunkPos) == loadedTileChunks.end()) {
			loadChunk(chunkPos);
		}
	}
}

FIntPoint ATileManager::worldToChunkPosition(const FVector& worldPosition) const {
	int32 chunkX = FMath::Floor(worldPosition.X / (tileHorizontalOffset * CHUNK_SIZE));
	int32 chunkY = FMath::Floor(worldPosition.Y / (tileVerticalOffset * CHUNK_SIZE));
	return FIntPoint(chunkX, chunkY);
}

void ATileManager::loadChunk(const FIntPoint& chunkPos) {
	if (loadedTileChunks.find(chunkPos) != loadedTileChunks.end()) {
		return;
	}
	const double points[30][2] = {
		{-38.96, -55.36},
		{40.96, 35.34},
		{-48.73, 1.07},
		{25.96, 8.99},
		{79.19, -32.68},
		{60.19, 20.75},
		{6.27, 94.62},
		{-21.04, 10.41},
		{-72.78, -53.44},
		{23.5, -27.03},
		{-56.96, 45.83},
		{-58.3, -24.11},
		{84.79, 28.0},
		{9.86, 36.92},
		{59.38, 55.2},
		{71.8, -8.23},
		{10.63, -47.45},
		{-17.42, -56.14},
		{-67.61, 25.49},
		{50.59, -15.57},
		{-84.61, 44.14},
		{-40.39, 28.19},
		{65.1, -47.32},
		{18.87, -69.43},
		{-86.5, -35.17},
		{-71.76, -2.8},
		{-40.69, 74.49},
		{-4.73, 56.92},
		{-16.87, -32.14},
		{8.72, -89.88}};
		
	ChunkData newChunk;
	newChunk.chunkPosition = chunkPos;
	newChunk.isLoaded = true;

	newChunk.chunkMesh = NewObject<UInstancedStaticMeshComponent>(this);
	newChunk.chunkMesh->SetStaticMesh(hexMesh);
	newChunk.chunkMesh->SetMaterial(0, baseMaterial);
	newChunk.chunkMesh->NumCustomDataFloats = 1;
	newChunk.chunkMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	newChunk.chunkMesh->RegisterComponent();

	newChunk.mountainMesh = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
	newChunk.mountainMesh->SetStaticMesh(mountainLODMesh);
	newChunk.mountainMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	newChunk.mountainMesh->RegisterComponent();

	newChunk.pineForest = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
	newChunk.pineForest->SetStaticMesh(pineLODMesh);
	newChunk.pineForest->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	newChunk.pineForest->RegisterComponent();

	for (int32 localY = 0; localY < CHUNK_SIZE; ++localY) {
		for (int32 localX = 0; localX < CHUNK_SIZE; ++localX) {
			int32 globalX = chunkPos.X * CHUNK_SIZE + localX;
			int32 globalY = chunkPos.Y * CHUNK_SIZE + localY;

			if (globalX >= 0 && globalX < gridWidth && globalY >= 0 && globalY < gridHeight) {
				bool flag = true;

				FTransform tileTransform = calculateTileTransform(globalX, globalY);
				newChunk.chunkMesh->AddInstance(tileTransform);

				const BiomeType& biome = tileBiomes[globalY * gridWidth + globalX];
				newChunk.chunkMesh->SetCustomDataValue(
					localY * CHUNK_SIZE + localX,
					0,
					biome == BiomeType::GRASSLAND ? 0.0f : 1.0f,
					true);

				if (FMath::RandRange(0, 10) == 5) {
					FTransform hillTransform = calculateHillTransform(globalX, globalY);
					newChunk.mountainMesh->AddInstance(hillTransform);
					flag = false;
				}
			
				if (flag) {
					for (size_t i = 0; i < 30; i++)
					{
						FTransform pineTransform = calculateTreeTransform(globalX, globalY, points[i][0], points[i][1]);
						newChunk.pineForest->AddInstance(pineTransform);
					}
				}
			}
		}
	}

	loadedTileChunks.emplace(chunkPos, std::move(newChunk));
}

void ATileManager::unloadChunk(const FIntPoint& chunkPos) {
	auto it = loadedTileChunks.find(chunkPos);
	if (it != loadedTileChunks.end()) {
		ChunkData& chunk = it->second;
		if (chunk.chunkMesh) {
			chunk.chunkMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			chunk.chunkMesh->DestroyComponent();
			chunk.chunkMesh = nullptr;
		}
		if (chunk.mountainMesh) {
			chunk.mountainMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			chunk.mountainMesh->DestroyComponent();
			chunk.mountainMesh = nullptr;
		}
		if (chunk.pineForest) {
			chunk.pineForest->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			chunk.pineForest->DestroyComponent();
			chunk.pineForest = nullptr;
		}
		loadedTileChunks.erase(it);
	}
}

bool ATileManager::isChunkInRenderDistance(const FIntPoint& chunkPos, const FIntPoint& playerChunkPos) const {
	float distance = FVector2D(chunkPos.X - playerChunkPos.X, chunkPos.Y - playerChunkPos.Y).Size();
	return distance <= renderDistance;
}

void ATileManager::BeginPlay() {
	Super::BeginPlay();
	loadAssets();

	if (!hexMesh || !mountainLODMesh || !baseMaterial || !pineLODMesh) {
		return;
	}

	for (size_t i = 0; i < (static_cast<uint32_t>(gridHeight) * static_cast<uint32_t>(gridWidth)); i++) {
		tileBiomes[i] = FMath::RandBool() ? BiomeType::GRASSLAND : BiomeType::CHAPPARAL;
		occupancyMasks[i].setAllTrue();
	}

	if (AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		setPlayerPosition(player->GetActorLocation());
	}
}

void ATileManager::loadAssets() {
	hexMeshAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/Hex.Hex")));
	baseMaterialAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/HexMaterial.HexMaterial")));
	mountainLODAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/HillLOD.HillLOD")));
	pineLODAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/Pine.Pine")));

	if (UStaticMesh* loadedMesh = hexMeshAsset.LoadSynchronous()) {
		hexMesh = loadedMesh;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load hex mesh!"));
	}
	if (UMaterialInterface* loadedMaterial = baseMaterialAsset.LoadSynchronous()) {
		baseMaterial = loadedMaterial;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load base material!"));
	}

	if (UStaticMesh* loadedMesh = mountainLODAsset.LoadSynchronous()) {
		mountainLODMesh = loadedMesh;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainAsset mesh!"));
	}

	if (UStaticMesh* loadedMesh = pineLODAsset.LoadSynchronous()) {
		pineLODMesh = loadedMesh;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load pineLODAsset mesh!"));
	}
}