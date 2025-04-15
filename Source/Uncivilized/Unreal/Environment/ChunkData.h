#pragma once

struct ChunkData {
	bool isLoaded = false;
	FIntPoint chunkPosition;
	UInstancedStaticMeshComponent* chunkMesh;
	UHierarchicalInstancedStaticMeshComponent* mountainHighPoly;
	UHierarchicalInstancedStaticMeshComponent* mountainMiddlePoly;
	UHierarchicalInstancedStaticMeshComponent* mountainLowPoly;

	ChunkData() : isLoaded(false), chunkMesh(nullptr) {}

	~ChunkData() {
		chunkMesh = nullptr;
	}

	ChunkData(const ChunkData&) = delete;
	ChunkData& operator=(const ChunkData&) = delete;

	ChunkData(ChunkData&& other) noexcept
		: chunkPosition(std::move(other.chunkPosition)),
		  chunkMesh(std::exchange(other.chunkMesh, nullptr)),
		  mountainHighPoly(std::exchange(other.mountainHighPoly, nullptr)),
		  mountainMiddlePoly(std::exchange(other.mountainMiddlePoly, nullptr)),
		  mountainLowPoly(std::exchange(other.mountainLowPoly, nullptr)),
		  isLoaded(std::exchange(other.isLoaded, false)) {}

	ChunkData& operator=(ChunkData&& other) noexcept {
		if (this != &other) {
			chunkPosition = std::move(other.chunkPosition);
			chunkMesh = std::exchange(other.chunkMesh, nullptr);
			mountainHighPoly = std::exchange(other.mountainHighPoly, nullptr);
			mountainMiddlePoly = std::exchange(other.mountainMiddlePoly, nullptr);
			mountainLowPoly = std::exchange(other.mountainLowPoly, nullptr);
			isLoaded = std::exchange(other.isLoaded, false);
		}
		return *this;
	}
};