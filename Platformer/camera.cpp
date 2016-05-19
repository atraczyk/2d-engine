//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	camera class
//-----------------------------------------------------------------------------

#include "camera.h"
#include "utils.h"
#include <math.h>

Camera::Camera()
{
    pTarget = NULL;
    scaleValue = 1.0f;
    aspect = 0.0f;
    toMove.x = 0;
    toMove.y = 0;
    isTargeting = false;
}

Camera::~Camera()
{
}

void Camera::initialize(int x, int y, int width, int height)
{
    position.x = x;
    position.y = y;
    size.x = width;
    size.y = height;
    velocity = Vector2(0.0f, 0.0f);
}

void Camera::setOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((int)(position.x), (int)(position.x + size.x),
        (int)(position.y), (int)(position.y + size.y),
        -1.0f, 1.0f);
}

void Camera::reshape(float w, float h)
{
    float dw = w - size.x;
    float dh = h - size.y;
    float cx = position.x + size.x / 2;
    float cy = position.y + size.y / 2;
    size.x += dw;
    size.y += dh;
    position.x = cx - size.x / 2;
    position.y = cy - size.y / 2;
}

void Camera::moveToTarget()
{
    position = pTarget->position + pTarget->size*0.5f - size*0.5f;
}

void Camera::update(WorldRect* pconstraints)
{
    if (isTargeting)
    {
        velocity = (((pTarget->position + pTarget->size*0.5f) - size*0.5f - position)*0.25f);

        if (abs(velocity.x) < 1.0f)
            velocity.x = 0.0f;
        if (abs(velocity.y) < 1.0f)
            velocity.y = 0.0f;
    }
    else
    {
        if (toMove.x == 0)
        {
            if (velocity.x < 0.0f)
            {
                velocity.x += DECELERATION;
                if (velocity.x > 0.0f)
                    velocity.x = 0.0f;
            }
            else if (velocity.x > 0.0f)
            {
                velocity.x -= DECELERATION;
                if (velocity.x < 0.0f)
                    velocity.x = 0.0f;
            }
        }
        else
        {
            if (velocity.x * toMove.x < 0)
                velocity.x += DECELERATION * toMove.x;
            else
                velocity.x += ACCELERATION * toMove.x;
            if (velocity.x < -VELOCITY_MAX)
                velocity.x = -VELOCITY_MAX;
            else if (velocity.x > VELOCITY_MAX)
                velocity.x = VELOCITY_MAX;
        }

        if (toMove.y == 0)
        {
            if (velocity.y < 0.0f)
            {
                velocity.y += DECELERATION;
                if (velocity.y > 0.0f)
                    velocity.y = 0.0f;
            }
            else if (velocity.y > 0.0f)
            {
                velocity.y -= DECELERATION;
                if (velocity.y < 0.0f)
                    velocity.y = 0.0f;
            }
        }
        else
        {
            if (velocity.y * toMove.y < 0)
                velocity.y += DECELERATION * toMove.y;
            else
                velocity.y += ACCELERATION * toMove.y;
            if (velocity.y < -VELOCITY_MAX)
                velocity.y = -VELOCITY_MAX;
            else if (velocity.y > VELOCITY_MAX)
                velocity.y = VELOCITY_MAX;
        }
    }

    lastPosition = position;
    position += velocity.Integer();

    if (position.x < pconstraints->fLeft())
        position.x = pconstraints->fLeft();
    if (position.x > pconstraints->fRight() - size.x)
        position.x = pconstraints->fRight() - size.x;
    if (position.y > pconstraints->fTop() - size.y)
        position.y = pconstraints->fTop() - size.y;
    if (position.y < pconstraints->fBottom())
        position.y = pconstraints->fBottom();
}