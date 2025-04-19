#pragma once

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

struct ChunkData {
	bool isLoaded = false;
	FIntPoint chunkPosition;
	UInstancedStaticMeshComponent* chunkMesh;
	UHierarchicalInstancedStaticMeshComponent* mountainMesh;
	UHierarchicalInstancedStaticMeshComponent* pineForest;

	ChunkData() : isLoaded(false),
				  chunkMesh(nullptr),
				  mountainMesh(nullptr),
				  pineForest(nullptr) {}

	~ChunkData() noexcept {
		chunkMesh = nullptr;
		mountainMesh = nullptr;
		pineForest = nullptr;
	}

	ChunkData(const ChunkData&) = delete;
	ChunkData& operator=(const ChunkData&) = delete;

	ChunkData(ChunkData&& other) noexcept
		: chunkPosition(std::move(other.chunkPosition)),
		  chunkMesh(std::exchange(other.chunkMesh, nullptr)),
		  mountainMesh(std::exchange(other.mountainMesh, nullptr)),
		  pineForest(std::exchange(other.pineForest, nullptr)),
		  isLoaded(std::exchange(other.isLoaded, false)) {}

	ChunkData& operator=(ChunkData&& other) noexcept {
		if (this != &other) {
			chunkPosition = std::move(other.chunkPosition);
			chunkMesh = std::exchange(other.chunkMesh, nullptr);
			mountainMesh = std::exchange(other.mountainMesh, nullptr);
			pineForest = std::exchange(other.pineForest, nullptr);
			isLoaded = std::exchange(other.isLoaded, false);
		}
		return *this;
	}
};