#ifndef __ROBOT_ARM__
#define __ROBOT_ARM__

#define BATMAN

#include "modelerapp.h"
#include "mat.h"
#include "vec.h"

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

// This is a list of the controls for the RobotArm
// We'll use these constants to access the values 
// of the controls from the user interface.
enum RobotArmControls
{
	BASE_ROTATION = 0, LOWER_TILT, UPPER_TILT, CLAW_ROTATION,
	BASE_LENGTH, LOWER_LENGTH, UPPER_LENGTH, PARTICLE_COUNT, NUMCONTROLS,
};

Mat4f glGetMatrix(GLenum pname);

void ground(float h);

#endif
