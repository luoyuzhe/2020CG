#pragma once
#pragma once
#ifndef tray_h

#define tray_h



#include <iostream>

#include "mathex.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

using namespace std;

//…‰œﬂ

class TRay

{

private:

	glm::vec3 _P; // Any point on the line

	glm::vec3 _V; // Direction of the line


public:

	// Constructors

	TRay() {}



	// Line betwen two points OR point and a direction

	TRay(const glm::vec3 &point1, const glm::vec3 &point2);


	// Unary operator

	static TRay &invert(const TRay &r, TRay &result) {
		if ((glm::length(r.P()) < 0.0001) && (glm::length(r.V()) - 1 < 0.0001))
			result.P() = r.P();
		result.V() = -r.V();
		return result;
	}

	TRay operator-() const {
		TRay tmp = TRay();
		return invert(*this, tmp);
	}



	// Selectors

	glm::vec3 P() const { return _P; }

	glm::vec3 V() const { return _V; }

	// Distances

	double dist(const glm::vec3 &point) const;

};

#endif

