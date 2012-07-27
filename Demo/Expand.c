/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include "chipmunk.h"
#include "chipmunk_unsafe.h"
#include "ChipmunkDemo.h"
#include <stdio.h>

#define GRAVITY 2000.0

static cpSpace *space;

static float height = 50;
static float width = 400;
static float expandSpeed = 0;
static cpBody *expandBody = NULL;
static cpShape *expandShape = NULL;

static cpBool
preSolve(cpArbiter *arb, cpSpace *space, void *ignore)
{
  CP_ARBITER_GET_BODIES(arb, expander, other);
  int count = cpArbiterGetCount(arb);
  for(int i = 0; i < count; i++)
  {
    float pointSpeed = (cpBodyWorld2Local(expander, cpArbiterGetPoint(arb, i)).x / (width/2)) * (expandSpeed/6);
    cpArbiterSetSurfaceVelocity(arb, i, cpv(-pointSpeed, 0));
  }

  return cpTrue;
}

static void
update(int ticks)
{
	// Step the space
	int steps = 3;
	cpFloat dt = 1.0f/60.0f/(cpFloat)steps;

	cpVect verts[] = {
		cpv(-width/2,-height/2),
		cpv(-width/2, height/2),
		cpv( width/2, height/2),
		cpv( width/2,-height/2),
	};

        cpPolyShapeSetVerts(expandShape, 4, verts, cpvzero);

        expandSpeed = 500*ChipmunkDemoKeyboard.x;
        width += expandSpeed*dt;

	for(int i=0; i<steps; i++){
		cpSpaceStep(space, dt);
	}
}

static cpSpace *
init(void)
{
	space = cpSpaceNew();
	space->iterations = 10;
	space->gravity = cpv(0, -GRAVITY);
//	space->sleepTimeThreshold = 1000;
	space->enableContactGraph = cpTrue;

	cpBody *body, *staticBody = space->staticBody;
	cpShape *shape;
	
	// Create segments around the edge of the screen.
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(320,-240), cpv(320,240), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,240), cpv(320,240), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	
	// Set up the box
	cpFloat radius = 25.0f;
	body = cpSpaceAddBody(space, cpBodyNew(1.0f, INFINITY));
        expandBody = body;
	cpBodySetPos(body, cpv(0, -200));

	shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 50, 50));
        expandShape = shape;
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.9f);
        cpShapeSetCollisionType(shape, 1);
	
	// Drop some boxes
	for(int i=0; i<6; i++){
          body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1, 50, 50)));
          cpBodySetPos(body, cpv(-150 + i*60, 150));
          
          shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 50, 50));
          cpShapeSetElasticity(shape, 0.0f);
          cpShapeSetFriction(shape, 0.9f);
          cpShapeSetCollisionType(shape, 2);
	}
	
	cpSpaceAddCollisionHandler(space, 1, 2, NULL, preSolve, NULL, NULL, NULL);

	return space;
}

static void
destroy(void)
{
	ChipmunkDemoFreeSpaceChildren(space);
	cpSpaceFree(space);
}

ChipmunkDemo Expand = {
	"Changing Shape",
	init,
	update,
	ChipmunkDemoDefaultDrawImpl,
	destroy,
};
