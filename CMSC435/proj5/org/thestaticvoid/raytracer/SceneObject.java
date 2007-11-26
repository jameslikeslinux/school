/*
 * SceneObject.java
 * James Lee <jlee23@umbc.edu>
 * 
 * All objects in the scene as parsed from the NFF file should implement
 * these functions necessary for ray-tracing.
 */
package org.thestaticvoid.raytracer;

import java.util.*;

public abstract class SceneObject {
	protected Scene scene;
	protected Fill fill;
	
	public SceneObject(Scene scene) {
		this.scene = scene;
		fill = scene.getCurrentFill();
	}
	
	public abstract Intersection intersects(Vector3 rayPosition, Vector3 rayDirection);
	public abstract Vector3 getNormal(Intersection intersection);
	
	public Fill getFill() {
		return fill;
	}
	
	public Vector3 getColor(Intersection intersection, int depth) {
		Vector3 normal = getNormal(intersection);
		Vector3 intersectionPoint = intersection.getPoint();
		Vector3 localColor = new Vector3(0.0, 0.0, 0.0);
		
		// Spawn shadow rays
		for (Light light : scene.getLights()) {
			Vector3 direction = light.getPosition().subtract(intersectionPoint);
			Intersection nearestIntersection = scene.shootRay(intersectionPoint, direction, intersection.getFillsPassedThrough());
			
			if (nearestIntersection == null || nearestIntersection.getPoint().subtract(intersectionPoint).length() > direction.length()) {				
				double diffuse = Math.max(0.0, normal.dotProduct(direction.normalize()));
				
				Vector3 viewVector = scene.getViewAt().subtract(intersectionPoint).normalize();
				Vector3 reflectionVector = normal.scale(2).scale(normal.dotProduct(direction)).subtract(direction).normalize();
				double specular = Math.pow(Math.max(0.0, reflectionVector.dotProduct(viewVector)), fill.getShine());
				
				localColor = localColor.add(fill.getColor().scale(fill.getDiffuseComponent() * diffuse).add(new Vector3(fill.getSpecularComponent() * specular)));
			}
		}
		
		// Spawn reflection ray
		if (fill.getSpecularComponent() > 0) {
			Vector3 incomingRay = intersection.getIncomingRay().scale(-1);
			Vector3 reflectionVector = normal.scale(2).scale(normal.dotProduct(incomingRay)).subtract(incomingRay).normalize();
			localColor = localColor.add(scene.getColorAtIntersection(scene.shootRay(intersectionPoint, reflectionVector, intersection.getFillsPassedThrough()), depth + 1).scale(fill.getSpecularComponent()));
		}
		
		// Spawn refraction ray
		if (fill.getTransmittance() > 0) {
			Stack<Fill> fillsPassedThrough = intersection.getFillsPassedThrough();
			
			double indexFrom = 0.0, indexTo = 0.0;
			
			/*
			 * if the stack is empty, then we are entering an object
			 * if the stack has one element, then we are leaving this object
			 * otherwise
			 * if the stack's top is this object then we are entering it
			 * finally we must be leaving this object
			 */
			if (fillsPassedThrough.size() == 0) {
				indexFrom = fill.getRefractionIndex();
				indexTo = 1.0;
			} else if (fillsPassedThrough.size() == 1) {
				indexFrom = 1.0;
				indexTo = fill.getRefractionIndex();
			} else {
				if (fillsPassedThrough.peek() == fill) {
					indexFrom = fillsPassedThrough.get(fillsPassedThrough.size() - 2).getRefractionIndex();
					indexTo = fill.getRefractionIndex();
				} else {
					indexFrom = fill.getRefractionIndex();
					indexTo = fillsPassedThrough.peek().getRefractionIndex();
				}
			}
			
			double indexRatio = indexFrom / indexTo;

			Vector3 incomingRay = intersection.getIncomingRay().scale(-1);
			double normalIncomingDotProd = normal.dotProduct(incomingRay);
			Vector3 refractionVector = normal.scale(indexRatio * normalIncomingDotProd - Math.sqrt(1 - indexRatio * indexRatio * normalIncomingDotProd * normalIncomingDotProd)).subtract(incomingRay.scale(indexRatio));
			
			Intersection refractionIntersection = scene.shootRay(intersectionPoint, refractionVector, fillsPassedThrough);
			localColor = localColor.add(scene.getColorAtIntersection(refractionIntersection, depth + 1).scale(fill.getTransmittance()));
		}
		
		// Prevent any color from looping around
		if (localColor.getX() > 1.0)
			localColor.setX(1.0);
		if (localColor.getY() > 1.0)
			localColor.setY(1.0);
		if (localColor.getZ() > 1.0)
			localColor.setZ(1.0);
		
		return localColor;
	}
}
