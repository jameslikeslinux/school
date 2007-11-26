/*
 * Sphere.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Shere scene object made up of a position and radius
 */
package org.thestaticvoid.raytracer;

public class Sphere extends SceneObject {
	private Vector3 center;
	private double radius, radiusSq;
	
	public Sphere(Scene scene, Vector3 center, double radius) {
		super(scene);
		this.center = center;
		this.radius = radius;
		radiusSq = radius * radius;
	}
	
	public Vector3 getNormal(Intersection intersection) {
		return intersection.getPoint().subtract(center).normalize();
	}
	
	/*
	 * Based on code at:
	 * http://www.devmaster.net/wiki/Ray-sphere_intersection
	 */
	public Intersection intersects(Vector3 rayPosition, Vector3 rayDirection) {
		rayDirection = rayDirection.normalize();
		Vector3 dst = rayPosition.subtract(center);
		double b = dst.dotProduct(rayDirection);
		double c = dst.dotProduct(dst) - radiusSq;
		double d = b * b - c;
		
		if (d > 0) {
			double t = -b - Math.sqrt(d);
			Vector3 intersectionPoint = rayPosition.add(rayDirection.scale(t));
			return new Intersection(this, t, intersectionPoint, rayDirection);
		}
		
		return null;
	}
}