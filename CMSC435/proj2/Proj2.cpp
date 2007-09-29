#include "Triangle.h"

int main() {
	Point lf(-1.0, 0.0, 1.0);
	Point ln(-1.0, 0.0, -1.0);
	Point rf(1.0, 0.0, 1.0);
	Point rn(1.0, 0.0, -1.0);

	Triangle triangle1(&lf, &rn, &ln);
	Triangle triangle2(&lf, &rn, &rf);
}
