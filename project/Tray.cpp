/*******************************************************************************/
/*********************************21/09/200*************************************/
/**********************Programmer: Dimitrios Christopoulos**********************/
/**********************for the oglchallenge contest*****************************/
/**********************COLLISION CRAZY******************************************/
/*******************************************************************************/

#include "tray.h"

#include <math.h>

#include "mathex.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>


// Line betwen two points OR point and a direction
TRay::TRay(const glm::vec3 &point1, const glm::vec3 &point2)
	: _P(point1) {
	glm::vec3 point = point2 - point1;
	point = glm::normalize(point);
	_V = ((glm::length(point2) - 1) < 0.0001) ? point2 : point;

}

// Distance between a ray and a point

double TRay::dist(const glm::vec3 &point) const {

	if (((glm::length(this->V()) - 1) < 0.0001) && (glm::length(this->P()) < 0.0001) && (glm::length(point) < 0.0001)) {

		glm::vec3 tv, point2;

		double lambda = glm::dot(_V, point - _P);

		tv = _V;
		tv.x *= lambda;
		tv.y *= lambda;
		tv.z *= lambda;
		point2 = _P + tv;

		return glm::distance(point, point2);

	}

	return 0.0;

}



