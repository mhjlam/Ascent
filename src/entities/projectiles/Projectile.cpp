#include "stdafx.h"
#include "Projectile.h"

#include "../../Common.h"
#include "../enemies/Enemy.h"

Projectile::Projectile(Ogre::Entity* const entity, Ogre::SceneNode* const node, const Ogre::Vector3& direction, const Ogre::Quaternion& orientation) : GameEntity(entity, node),
mOrientation(orientation), mDamage(0), mSpeed(0), mRange(0)
{
    setDirection(direction);
}

void Projectile::update(const Ogre::Real& elapsed)
{
    // check for collision
    Ogre::Ray frontRay(getPosition(), getDirection());

	Common::mRaySceneQuery->setRay(frontRay);
	Common::mRaySceneQuery->setSortByDistance(true, 5);
    Common::mRaySceneQuery->setQueryMask(COLLIDABLE_WALL | COLLIDABLE_ENEMY);
	
	Ogre::RaySceneQueryResult::iterator it;
	Ogre::RaySceneQueryResult& result = Common::mRaySceneQuery->execute();

	for (it = result.begin(); it != result.end(); ++it)
	{
		if (it->movable->getQueryFlags() == COLLIDABLE_WALL && it->distance <= 100.0f) // hit a wall
		{
			destroyMe();
			return;
		}
		else if (it->movable->getQueryFlags() == COLLIDABLE_ENEMY && it->distance <= 300.0f) // hit an enemy
		{
            if (isType(ENT_PROJECTILE_PLAYER))
            {
                // damage enemy
                if (it->movable->isAttached()) // this should always be the case, but check anyway
                {
                    if (it->movable->getParentNode()->getUserAny().isEmpty() == false)
                    {
                        // sadly, this cast doesn't support abstract types, so all info about any subclasses of Enemy is lost
                        Enemy* const e = Ogre::any_cast<Enemy*>((it->movable->getParentNode()->getUserAny()));
			    
                        if (e) e->hit(mDamage);        
                    }
                }

                destroyMe();
			    return;
            }
        } 
	}

    // if it didn't hit anything, move in the specified direction
    Ogre::Vector3 distance = getDirection() * (mSpeed * elapsed);
    mMainNode->translate(distance, Ogre::Node::TS_WORLD);

    // check if the projectile has reached its max range
    if (mMainNode->getPosition().squaredDistance(mStartPosition) > mRange*mRange)
        destroyMe();
}