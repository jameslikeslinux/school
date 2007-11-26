/*
 * Vector3.java
 * James Lee <jlee23@umbc.edu>
 * 
 * A basic representation of a three-dimensional vector.
 */
package org.thestaticvoid.raytracer;

public class Vector3 {
	private double x, y, z;
	
	public enum Component { X, Y, Z };
	
	public Vector3(double value) {
		this(value, value, value);
	}
	
	public Vector3(double x, double y, double z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public Vector3 add(Vector3 vector3) {
		return new Vector3(x + vector3.x, y + vector3.y, z + vector3.z);
	}
	
	public Vector3 subtract(Vector3 vector3) {
		return new Vector3(x - vector3.x, y - vector3.y, z - vector3.z);
	}
	
	public Vector3 scale(double scale) {
		return new Vector3(x * scale, y * scale, z * scale);
	}
	
	public double dotProduct(Vector3 vector3) {
		return x * vector3.x + y * vector3.y + z * vector3.z;
	}
	
	public Vector3 crossProduct(Vector3 vector3) {
		return new Vector3(y * vector3.z - z * vector3.y, z * vector3.x - x * vector3.z, x * vector3.y - y * vector3.x);
	}
	
	public double length() {
		return Math.sqrt(x * x + y * y + z * z);
	}
	
	public Vector3 normalize() {
		double length = length();
		if (length == 0)
			length = 1;
		
		return scale(1 / length);
	}
	
	public double getX() {
		return x;
	}

	public void setX(double x) {
		this.x = x;
	}
	
	public double getY() {
		return y;
	}

	public void setY(double y) {
		this.y = y;
	}
	
	public double getZ() {
		return z;
	}

	public void setZ(double z) {
		this.z = z;
	}
	
	public Vector2 toVector2(Component dropComponent) {
		if (dropComponent == Component.X)
			return new Vector2(y, z);
		else if (dropComponent == Component.Y)
			return new Vector2(x, z);
		else
			return new Vector2(x, y);
	}
	
	public String toString() {
		return "[" + x + ", " + y + ", " + z + "]";
	}
}
