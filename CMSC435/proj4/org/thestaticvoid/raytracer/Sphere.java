/*
 * Sphere.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Shere scene object made up of a position and radius
 */
package org.thestaticvoid.raytracer;

public class Sphere implements SceneObject {
	private Vector3 fillColor, center;
	private double radius, radiusSq;
	
	public Sphere(Vector3 fillColor, Vector3 center, double radius) {
		this.fillColor = fillColor;
		this.center = center;
		this.radius = radius;
		radiusSq = radius * radius;
	}
	
	public Vector3 getColor() {
		return fillColor;
	}
	
	/*
	 * Based on code at:
	 * http://www.devmaster.net/wiki/Ray-sphere_intersection
	 */
	public double intersects(Vector3 rayPosition, Vector3 rayDirection) {
		Vector3 oc = center.subtract(rayPosition);
		double l20c = oc.dotProduct(oc);
		
		if (l20c < radiusSq) {
			double tca = oc.dotProduct(rayDirection) / rayDirection.dotProduct(rayDirection);
			double l2hc = (radiusSq - l20c) / rayDirection.dotProduct(rayDirection) + tca * tca;
			return tca + Math.sqrt(l2hc);
		} else {
			double tca = oc.dotProduct(rayDirection);
			if (tca < 0)
				return NO_INTERSECTION;
			double l2hc = (radiusSq - l20c) / rayDirection.dotProduct(rayDirection) + (tca * tca);
			return l2hc > 0 ? tca - Math.sqrt(l2hc) : NO_INTERSECTION;
		}
	}
}