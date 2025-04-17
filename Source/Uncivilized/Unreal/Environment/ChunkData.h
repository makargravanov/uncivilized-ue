#pragma once

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

struct ChunkData {
	bool isLoaded = false;
	FIntPoint chunkPosition;
	UInstancedStaticMeshComponent* chunkMesh;
	UHierarchicalInstancedStaticMeshComponent* mountainMesh;

	ChunkData() : isLoaded(false), chunkMesh(nullptr) {}

	~ChunkData() {
		chunkMesh = nullptr;
		mountainMesh = nullptr;
	}

	ChunkData(const ChunkData&) = delete;
	ChunkData& operator=(const ChunkData&) = delete;

	ChunkData(ChunkData&& other) noexcept
		: chunkPosition(std::move(other.chunkPosition)),
		  chunkMesh(std::exchange(other.chunkMesh, nullptr)),
		  mountainMesh(std::exchange(other.mountainMesh, nullptr)),
		  isLoaded(std::exchange(other.isLoaded, false)) {}

	ChunkData& operator=(ChunkData&& other) noexcept {
		if (this != &other) {
			chunkPosition = std::move(other.chunkPosition);
			chunkMesh = std::exchange(other.chunkMesh, nullptr);
			mountainMesh = std::exchange(other.mountainMesh, nullptr);
			isLoaded = std::exchange(other.isLoaded, false);
		}
		return *this;
	}
};