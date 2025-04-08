#pragma once


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