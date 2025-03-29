// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

#define CHUNK_SIZE 64

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
	MOUND = 0,
	LOW = 1,
	MEDUIM = 2,
	HIGH = 3
};

struct ChunkData {
	FIntPoint chunkPosition;
	UInstancedStaticMeshComponent* chunkMesh;
	bool isLoaded;
	ChunkData() : isLoaded(false), chunkMesh(nullptr) {}

	~ChunkData(){
		delete chunkMesh;
	}
};


UCLASS()
class UNCIVILIZED_API ATileManager : public AActor {
	GENERATED_BODY()

  public:
	// Sets default values for this actor's properties
	ATileManager();

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "HexGrid|Materials")
	UMaterialInterface* baseMaterial;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* hexMeshInstances;

  public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  private:
	TMap<FIntPoint, ChunkData> loadedTileChunks;
	TMap<FIntPoint, ChunkData> loadedHillChunks;
	TMap<FIntPoint, ChunkData> loadedForestChunks;

	float tileHorizontalOffset = 173.205078;
	float oddRowHorizontalOffset = 86.602539;
	float tileVerticalOffset = 150.0;

	BiomeType* tileBiomes;

	uint8_t* heights = nullptr;

	uint8_t* rotationsTileHillBitMasks = nullptr; 
	// | 1,2,3,4 | 5,6,7,8 |
	uint8_t* rotationsForestBuildingBitMasks = nullptr; 
	// | 1,2,3,4 | 5,6,7,8 |
	uint8_t* roadRailroadIsElectrificationIsSwampBitMasks = nullptr; 
	// | 1,2,3 | 4,5,6 | 7 | 8 |

	uint8_t* 
	
	uint16_t height;
	uint16_t width;
};
