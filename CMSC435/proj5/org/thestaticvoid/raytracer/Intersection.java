/*
 * Intersection.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Represents a ray-object intersection.
 */
package org.thestaticvoid.raytracer;

import java.util.*;

public class Intersection {
	private SceneObject sceneObject;
	private double t;
	private Vector3 point;
	private Vector3 incomingRay;
	private Stack<Fill> fillsPassedThrough;
	
	public Intersection(SceneObject sceneObject, double t, Vector3 point, Vector3 incomingRay) {
		this.sceneObject = sceneObject;
		this.t = t;
		this.point = point;
		this.incomingRay = incomingRay;
	}
	
	public SceneObject getSceneObject() {
		return sceneObject;
	}
	
	public double getT() {
		return t;
	}
	
	public Vector3 getPoint() {
		return point;
	}
	
	public Vector3 getIncomingRay() {
		return incomingRay.normalize();
	}
	
	public Stack<Fill> getFillsPassedThrough() {
		return fillsPassedThrough;
	}
	
	public void setFillsPassedThrough(Stack<Fill> fillsPassedThrough) {
		this.fillsPassedThrough = fillsPassedThrough;
	}
}
