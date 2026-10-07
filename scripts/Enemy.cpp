#include "Enemy.h"

#include "GameState.h"

#include "Action.h"
#include "ActionComponent.h"
#include "Engine.h"
#include "Object.h"

using namespace doriax;

Enemy::Enemy(Scene* scene, Entity entity): ScriptBase(scene, entity) {
    REGISTER_ENGINE_EVENT(onUpdate);
}

Enemy::~Enemy() {
    UNREGISTER_ENGINE_EVENT(onUpdate);
}

// squash() runs in a contact callback, the bomb is flattened here
void Enemy::onUpdate() {
    if (!squashed || GameState::paused) return;

    if (!flattened) {
        flattened = true;

        // stays where it was squashed: its patrol and waddle stop
        auto actions = scene->getComponentArray<ActionComponent>();
        for (size_t i = 0; i < actions->size(); i++) {
            Entity target = actions->getComponentFromIndex(i).target;
            if (target == entity || scene->isParentOf(entity, target)) {
                Action(scene, actions->getEntity(i)).stop();
            }
        }

        Entity visual = scene->findEntity("Visual", entity);
        if (visual != NULL_ENTITY) {
            Object flat(scene, visual);
            Vector3 scale = flat.getScale();
            flat.setScale(Vector3(scale.x * 1.3f, scale.y * 0.3f, scale.z * 1.3f));
        }
    }

    squashTimer += Engine::getDeltatime();
    if (squashTimer > 0.5f) {
        Object(scene, entity).setVisible(false);
    }
}

void Enemy::squash() {
    squashed = true;
}
