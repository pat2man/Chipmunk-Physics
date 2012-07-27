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

// Using the change in scale seemed easier to use in calculations.
// Storing the x/y scale separately to better match your demo.
static cpVect expandScaleDt = {1.0, 1.0};

static cpBody *expandBody = NULL;
static cpShape *expandShape = NULL;

static cpBool
preSolve(cpArbiter *arb, cpSpace *space, void *ignore)
{
  CP_ARBITER_GET_BODIES(arb, expander, other);
	
	cpFloat inv_dt = 1.0/cpSpaceGetCurrentTimeStep(space);
	
  int count = cpArbiterGetCount(arb);
  for(int i = 0; i < count; i++)
  {
		// So at any particular contact point, the surface would be moving at localPoint*(1.0 - expandScaleDt)/dt (I think?)
		cpVect localPoint = cpBodyWorld2Local(expander, cpArbiterGetPoint(arb, i));
		cpVect surfaceV = cpv(localPoint.x*(1.0 - expandScaleDt.x)*inv_dt, localPoint.y*(expandScaleDt.y - 1.0)*inv_dt);
		
		// Just need to rotate the surface velocity back to absolute coords
		cpArbiterSetSurfaceVelocity(arb, i, cpvunrotate(cpBodyGetRot(expander), surfaceV));
  }
		ChipmunkDemoPrintString("\n");

  return cpTrue;
}

static void
update(int ticks)
{
	// Step the space
	int steps = 3;
	cpFloat dt = 1.0f/60.0f/(cpFloat)steps;
	

	for(int i=0; i<steps; i++){
		// This all needs to happen inside the fixed timestep.
		
		cpVect expandSpeed = cpv(100.0*ChipmunkDemoKeyboard.x*dt, 100*ChipmunkDemoKeyboard.y*dt);
		cpFloat newWidth = cpfmax(50.0, width + expandSpeed.x);
		cpFloat newHeight = cpfmax(50.0, height + expandSpeed.y);
		
		expandScaleDt = cpv(newWidth/width, newHeight/height);
		height = newHeight;
		width = newWidth;
//		width = cpfmax(50.0, width*expandScaleDt.x);
//		height = cpfmax(50.0, height*expandScaleDt.y);
		
		cpVect verts[] = {
			cpv(-width/2,-height/2),
			cpv(-width/2, height/2),
			cpv( width/2, height/2),
			cpv( width/2,-height/2),
		};
		
		cpPolyShapeSetVerts(expandShape, 4, verts, cpvzero);
		
		// I wanted to see how it reacted when recalculating the moment as well.
		cpBodySetMoment(expandBody, cpMomentForBox(cpBodyGetMass(expandBody), width, height));
		
		
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
	
	height = 50;
	width = 400;
	
	// Set up the box
	cpFloat radius = 25.0f;
	body = cpSpaceAddBody(space, cpBodyNew(10.0f, cpMomentForBox(10.0, width, height)));
        expandBody = body;
	cpBodySetPos(body, cpv(0, -200));

	shape = cpSpaceAddShape(space, cpBoxShapeNew(body, width, height));
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
          cpShapeSetCollisionType(shape, 0);
	}
	
	cpSpaceAddCollisionHandler(space, 1, 0, NULL, preSolve, NULL, NULL, NULL);

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
