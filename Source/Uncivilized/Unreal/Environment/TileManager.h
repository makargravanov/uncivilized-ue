// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <unordered_map>
#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

#define CHUNK_SIZE 64

template <>
struct std::hash<FIntPoint> {
	size_t operator()(const FIntPoint& Point) const noexcept {
		return static_cast<size_t>(Point.X) ^ (static_cast<size_t>(Point.Y) << 16);
	}
};

enum class BiomeType : uint8_t {
	OCEAN = 0,
	LAKE = 1,
	POLAR_ICE = 2,
	POLAR_DESERT = 3,
	TUNDRA = 4,
	FOREST_TUNDRA = 5,
	COLD_DESERT = 6,
	CONIFEROUS_FOREST = 7,
	DECIDUOUS_FOREST = 8,
	GRASSLAND = 9,
	STEPPES = 10,
	CHAPPARAL = 11,
	HOT_DESERT = 12,
	SAVANNA = 13,
	SUBTROPICAL_FOREST = 14,
	TROPICAL_SEASONAL_FOREST = 15,
	TROPICAL_RAIN_FOREST = 16,
};

enum class HillType : uint8_t {
	NONE = 0,
	MOUND = 1,
	LOW = 2,
	MEDUIM = 3,
	HIGH = 4
};

struct ChunkData {
	bool isLoaded = false;
	FIntPoint chunkPosition;
	UInstancedStaticMeshComponent* chunkMesh;

	ChunkData() : isLoaded(false), chunkMesh(nullptr) {}

	~ChunkData() {
		chunkMesh = nullptr;
	}

	ChunkData(const ChunkData&) = delete;
	ChunkData& operator=(const ChunkData&) = delete;

	ChunkData(ChunkData&& other) noexcept
		: chunkPosition(std::move(other.chunkPosition)),
		  chunkMesh(std::exchange(other.chunkMesh, nullptr)),
		  isLoaded(std::exchange(other.isLoaded, false)) {}

	ChunkData& operator=(ChunkData&& other) noexcept {
		if (this != &other) {
			chunkMesh = nullptr;

			chunkPosition = std::move(other.chunkPosition);
			chunkMesh = std::exchange(other.chunkMesh, nullptr);
			isLoaded = std::exchange(other.isLoaded, false);
		}
		return *this;
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

	std::unordered_map<FIntPoint, ChunkData> loadedTileChunks;
	std::unordered_map<FIntPoint, ChunkData> loadedHillChunks;

	BiomeType* tileBiomes = nullptr;
	uint8_t* heights = nullptr;

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

	UPROPERTY(EditAnywhere, Category = "HexGrid|Materials")
	UMaterialInterface* baseMaterial;
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* hexMeshInstances;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> hexMeshAsset;
	UPROPERTY()
	TSoftObjectPtr<UMaterialInterface> baseMaterialAsset;

  private:
	FTransform calculateTileTransform(const int32 x, const int32 y) const {
		const bool oddRow = y % 2 == 1;
		const float xPos = oddRow ? x * tileHorizontalOffset + oddRowHorizontalOffset : x * tileHorizontalOffset;
		const float yPos = y * tileVerticalOffset;
		return FTransform(FRotator::ZeroRotator, FVector(xPos, yPos, 0.0f));
	}

	void updateTileMaterial(const int32 instanceIndex, float num) const {
		hexMeshInstances->SetCustomDataValue(instanceIndex, 0, num, true);
		hexMeshInstances->MarkRenderStateDirty();
	}
};
