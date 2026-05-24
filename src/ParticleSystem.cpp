#include "ParticleSystem.h"
#include "Engine.h"
#include "Render.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem(int poolSize)
{
    pool.resize(poolSize);
    for (Particle& p : pool)
    {
        p.active = false;
    }
}

float ParticleSystem::RandomFloat(float min, float max)
{
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

int ParticleSystem::RandomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

ParticleEmitterConfig ParticleSystem::GetConfig(ParticleEmitterType type)
{
    ParticleEmitterConfig config;

    if (type == ParticleEmitterType::POISON)
    {
        config.type       = ParticleEmitterType::POISON;
        config.r          = 80;
        config.g          = 200;
        config.b          = 80;
        config.minSize    = 3;
        config.maxSize    = 6;
        config.minLife    = 600.0f;
        config.maxLife    = 1200.0f;
        config.speed      = 0.03f;
        config.emitRadius = 20.0f;
        config.emitRate   = 8;
    }
    else // FIRE
    {
        config.type       = ParticleEmitterType::FIRE;
        config.r          = 255;
        config.g          = 120;
        config.b          = 20;
        config.minSize    = 3;
        config.maxSize    = 7;
        config.minLife    = 300.0f;
        config.maxLife    = 700.0f;
        config.speed      = 0.06f;
        config.emitRadius = 15.0f;
        config.emitRate   = 12;
    }

    return config;
}

void ParticleSystem::Emit(const Vector2D& origin, ParticleEmitterType type, float dt)
{
    ParticleEmitterConfig config = GetConfig(type);

    float particlesThisFrame = config.emitRate * (dt / 1000.0f);
    int count = (int)particlesThisFrame;

    // Fraccion sobrante: emitir una particula extra probabilisticamente
    float remainder = particlesThisFrame - (float)count;
    if (RandomFloat(0.0f, 1.0f) < remainder)
    {
        count++;
    }

    for (int i = 0; i < count; i++)
    {
        int idx = GetInactiveParticle();
        if (idx == -1) { break; } // pool lleno, no hacer nada

        ResetParticle(idx, origin, config);
    }
}


void ParticleSystem::Update(float dt)
{
    for (Particle& p : pool)
    {
        if (!p.active) { continue; }

        p.life -= dt;

        p.position.setX(p.position.getX() + p.velocity.getX() * dt);
        p.position.setY(p.position.getY() + p.velocity.getY() * dt);

        if (p.life <= 0.0f)
        {
            p.active = false; // devuelta al pool, lista para reutilizar
        }
    }
}

void ParticleSystem::Draw() const
{
    for (const Particle& p : pool)
    {
        if (!p.active) { continue; }

        float lifeRatio = p.life / p.maxLife;
        Uint8 alpha = (Uint8)(lifeRatio * 200.0f);

        SDL_Rect rect;
        rect.x = (int)p.position.getX() - p.size / 2;
        rect.y = (int)p.position.getY() - p.size / 2;
        rect.w = p.size;
        rect.h = p.size;

        Engine::GetInstance().render->DrawRectangle(rect, p.r, p.g, p.b, alpha, true, false);
    }
}

void ParticleSystem::Clear()
{
    for (Particle& p : pool)
    {
        p.active = false;
    }
    pool.clear();
}

int ParticleSystem::GetInactiveParticle() const
{
    for (int i = 0; i < (int)pool.size(); i++)
    {
        if (!pool[i].active)
        {
            return i;
        }
    }
    return -1;
}

void ParticleSystem::ResetParticle(int index, const Vector2D& origin, const ParticleEmitterConfig& config)
{
    Particle& p = pool[index];

    float angle = RandomFloat(0.0f, 2.0f * 3.14159f);
    float radius = RandomFloat(0.0f, config.emitRadius);
    p.position.setX(origin.getX() + cosf(angle) * radius);
    p.position.setY(origin.getY() + sinf(angle) * radius);

    float vx = RandomFloat(-config.speed, config.speed);
    float vy = -RandomFloat(config.speed * 0.5f, config.speed * 2.0f);
    p.velocity.setX(vx);
    p.velocity.setY(vy);

    p.life = RandomFloat(config.minLife, config.maxLife);
    p.maxLife = p.life;
    p.size = RandomInt(config.minSize, config.maxSize);
    p.r = config.r;
    p.g = config.g;
    p.b = config.b;
    p.active = true;
}
