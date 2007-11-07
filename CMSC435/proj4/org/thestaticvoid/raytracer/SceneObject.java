/*
 * SceneObject.java
 * James Lee <jlee23@umbc.edu>
 * 
 * All objects in the scene as parsed from the NFF file should implement
 * these functions necessary for ray-tracing.
 */
package org.thestaticvoid.raytracer;

public interface SceneObject {
	public static final double NO_INTERSECTION = Double.NEGATIVE_INFINITY;
	public double intersects(Vector3 rayPosition, Vector3 rayDirection);
	public Vector3 getColor();
}
