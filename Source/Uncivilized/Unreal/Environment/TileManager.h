// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BiomeType.h"
#include "BitMatrix.h"
#include "ChunkData.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HillType.h"
#include <functional>
#include <unordered_map>
#include "TileManager.generated.h"

#define CHUNK_SIZE 64

template <>
struct std::hash<FIntPoint> {
	size_t operator()(const FIntPoint& Point) const noexcept {
		return static_cast<size_t>(Point.X) ^ (static_cast<size_t>(Point.Y) << 16);
	}
};

UCLASS()
class UNCIVILIZED_API ATileManager : public AActor {
	GENERATED_BODY()

  private:
	uint16_t gridHeight = 0;
	uint16_t gridWidth = 0;
	uint8_t renderDistance = 2;
	float tileHorizontalOffset = 173.205078;
	float oddRowHorizontalOffset = 86.602539;
	float tileVerticalOffset = 150.0;
	uint8_t numberOfTrees = 0;

	BitMatrix16x16* occupancyMasks = nullptr;

	std::unordered_map<FIntPoint, ChunkData> loadedTileChunks;

	BiomeType* tileBiomes = nullptr;
	uint8_t* uint8_t* heights = nullptr;

	uint8_t* rotationsTileHillTypeBitMasks = nullptr; // | 1,2,3,4 | 5,6,7,8 |
	uint8_t* rotationsForest = nullptr;

	FIntPoint playerChunkPosition;

  public:
	ATileManager();
	virtual void Tick(float DeltaTime) override;

  protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void updateVisibleChunks(const FVector& playerPosition);
	void loadChunk(const FIntPoint& chunkPos);
	void unloadChunk(const FIntPoint& chunkPos);
	bool isChunkInRenderDistance(const FIntPoint& chunkPos, const FIntPoint& playerChunkPos) const;
	FIntPoint worldToChunkPosition(const FVector& worldPosition) const;
	void setPlayerPosition(const FVector& newPosition);
	void loadAssets();

	UPROPERTY()
	UMaterialInterface* baseMaterial;
	UPROPERTY()
	UStaticMesh* hexMesh;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> hexMeshAsset;
	UPROPERTY()
	TSoftObjectPtr<UMaterialInterface> baseMaterialAsset;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> mountainLODAsset;
	UPROPERTY()
	UStaticMesh* mountainLODMesh;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> pineLODAsset;
	UPROPERTY()
	UStaticMesh* pineLODMesh;

  private:
	FTransform calculateTileTransform(const int32 x, const int32 y) const {
		const bool oddRow = y % 2 == 1;
		const float xPos = oddRow ? x * tileHorizontalOffset + oddRowHorizontalOffset : x * tileHorizontalOffset;
		const float yPos = y * tileVerticalOffset;
		return FTransform(FRotator::ZeroRotator, FVector(xPos, yPos, 0.0f));
	}

	FTransform calculateHillTransform(const int32 x, const int32 y) const {
		const bool oddRow = y % 2 == 1;
		const float xPos = oddRow ? x * tileHorizontalOffset + oddRowHorizontalOffset : x * tileHorizontalOffset;
		const float yPos = y * tileVerticalOffset;
		return FTransform(FRotator::ZeroRotator, FVector(xPos, yPos, 0.0f));
	}

	FTransform calculateTreeTransform(const int32 x, const int32 y) const {
		const bool oddRow = y % 2 == 1;
		const float xPos = oddRow ? x * tileHorizontalOffset + oddRowHorizontalOffset : x * tileHorizontalOffset;
		const float yPos = y * tileVerticalOffset;
		return FTransform(FRotator::ZeroRotator, FVector(xPos, yPos, 0.0f));
	}

	void updateTileMaterial(UInstancedStaticMeshComponent* instancedStaticMeshes, const int32 instanceIndex, float num) const {
		instancedStaticMeshes->SetCustomDataValue(instanceIndex, 0, num, true);
		instancedStaticMeshes->MarkRenderStateDirty();
	}
};
