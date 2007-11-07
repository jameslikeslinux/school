/*
 * Vector2.java
 * James Lee <jlee23@umbc.edu>
 * 
 * A basic representation of a two-dimensional vector.
 */
package org.thestaticvoid.raytracer;

public class Vector2 {
	private double x, y;
	
	public Vector2(double x, double y) {
		this.x = x;
		this.y = y;
	}
	
	public Vector2 subtract(Vector2 vector2) {
		return new Vector2(x - vector2.x, y - vector2.y);
	}
	
	public double getX() {
		return x;
	}
	
	public double getY() {
		return y;
	}
}
