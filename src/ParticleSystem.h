#pragma once
#include <vector>
#include "Vector2D.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

struct Particle
{
    Vector2D position;
    Vector2D velocity;
    float life;        // tiempo restante en ms
    float maxLife;     // vida total para calcular alpha
    int size;
    Uint8 r, g, b;
    bool active;       // true = visible y viva, false = en el pool esperando
    SDL_Texture* texture = nullptr;  // nullptr = dibuja rectángulo (fallback)
};

enum class ParticleEmitterType
{
    POISON,
    FIRE
};

struct ParticleEmitterConfig
{
    ParticleEmitterType type;
    Uint8 r, g, b;
    int minSize;
    int maxSize;
    float minLife;     // ms
    float maxLife;     // ms
    float speed;
    float emitRadius;  // radio alrededor del origen en el que aparecen
    int emitRate;      // particulas por segundo
    std::string texturePath;  // vacío = sin textura
};

class ParticleSystem
{
public:
    ParticleSystem(int poolSize = 200);
    ~ParticleSystem();

    void Update(float dt);
    void Draw() const;

    // Llama esto cada frame para un personaje que tenga el efecto activo
    void Emit(const Vector2D& origin, ParticleEmitterType type, float dt);

    void Clear();

private:
    std::vector<Particle> pool;
    SDL_Texture* poisonTexture = nullptr;
    SDL_Texture* fireTexture = nullptr;

    // Devuelve el indice de una particula inactiva del pool, o -1 si el pool esta lleno
    int GetInactiveParticle() const;

    // Reinicia una particula del pool con nuevos valores
    void ResetParticle(int index, const Vector2D& origin, const ParticleEmitterConfig& config);
    void LoadTextures();

    static ParticleEmitterConfig GetConfig(ParticleEmitterType type);
    static float RandomFloat(float min, float max);
    static int RandomInt(int min, int max);
    static constexpr int MAX_PARTICLES = 500;
};
