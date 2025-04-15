#include "TileManager.h"
#include <Kismet/GameplayStatics.h>

ATileManager::ATileManager() {
	PrimaryActorTick.bCanEverTick = true;

	this->tileBiomes = new BiomeType[2048 * 2048]{};
	gridHeight = 2048;
	gridWidth = 2048;
}

void ATileManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	delete[] this->tileBiomes;
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

	ChunkData newChunk;
	newChunk.chunkPosition = chunkPos;
	newChunk.isLoaded = true;

	newChunk.chunkMesh = NewObject<UInstancedStaticMeshComponent>(this);
	newChunk.chunkMesh->SetStaticMesh(hexMesh);
	newChunk.chunkMesh->SetMaterial(0, baseMaterial);
	newChunk.chunkMesh->NumCustomDataFloats = 1;
	newChunk.chunkMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	newChunk.chunkMesh->RegisterComponent();

	for (int32 localY = 0; localY < CHUNK_SIZE; ++localY) {
		for (int32 localX = 0; localX < CHUNK_SIZE; ++localX) {
			int32 globalX = chunkPos.X * CHUNK_SIZE + localX;
			int32 globalY = chunkPos.Y * CHUNK_SIZE + localY;

			if (globalX >= 0 && globalX < gridWidth && globalY >= 0 && globalY < gridHeight) {
				FTransform tileTransform = calculateTileTransform(globalX, globalY);
				newChunk.chunkMesh->AddInstance(tileTransform);

				const BiomeType& biome = tileBiomes[globalY * gridWidth + globalX];
				newChunk.chunkMesh->SetCustomDataValue(
					localY * CHUNK_SIZE + localX,
					0,
					biome == BiomeType::GRASSLAND ? 0.0f : 1.0f,
					true);
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

	if (!hexMesh || !baseMaterial)
		return;

	for (size_t i = 0; i < (static_cast<uint32_t>(gridHeight) * static_cast<uint32_t>(gridWidth)); i++) {
		tileBiomes[i] = FMath::RandBool() ? BiomeType::GRASSLAND : BiomeType::CHAPPARAL;
	}

	if (AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		setPlayerPosition(player->GetActorLocation());
	}
}

void ATileManager::loadAssets() {
	hexMeshAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/Hex.Hex")));
	baseMaterialAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/HexMaterial.HexMaterial")));

	mountainHighPolyAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/HillHighPoly.HillHighPoly")));
	mountainHighPolyMaterialAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/HillHighPolyMaterial.HillHighPolyMaterial")));

	mountainMiddlePolyAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/HillMiddlePoly.HillMiddlePoly")));
	mountainMiddlePolyMaterialAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/HillHighMiddlePolyMaterial.HillHighMiddlePolyMaterial")));

	mountainLowPolyAsset = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Meshes/HillLowPoly.HillLowPoly")));
	mountainLowPolyMaterialAsset = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/HillHighLowPolyMaterial.HillHighLowPolyMaterial")));

	if (UStaticMesh* loadedMesh = hexMeshAsset.LoadSynchronous()) {
		hexMesh = loadedMesh
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

	if (UStaticMesh* loadedMesh = mountainHighPolyAsset.LoadSynchronous()) {
		mountainHighPolyMesh = loadedMesh
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainHighPolyAsset mesh!"));
	}
	if (UMaterialInterface* loadedMaterial = mountainHighPolyMaterialAsset.LoadSynchronous()) {
		mountainHighPolyMaterial = loadedMaterial;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainHighPolyMaterialAsset material!"));
	}

	if (UStaticMesh* loadedMesh = mountainMiddlePolyAsset.LoadSynchronous()) {
		mountainMiddlePolyMesh = loadedMesh
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainMiddlePolyAsset mesh!"));
	}
	if (UMaterialInterface* loadedMaterial = mountainMiddlePolyMaterialAsset.LoadSynchronous()) {
		mountainMiddlePolyMaterial = loadedMaterial;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainMiddlePolyMaterialAsset material!"));
	}

	if (UStaticMesh* loadedMesh = mountainLowPolyAsset.LoadSynchronous()) {
		mountainLowPolyMesh = loadedMesh
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainLowPolyAsset mesh!"));
	}
	if (UMaterialInterface* loadedMaterial = mountainLowPolyMaterialAsset.LoadSynchronous()) {
		mountainLowPolyMaterial = loadedMaterial;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to load mountainLowPolyMaterialAsset material!"));
	}
}