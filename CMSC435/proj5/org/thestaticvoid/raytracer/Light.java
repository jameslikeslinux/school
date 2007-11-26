package org.thestaticvoid.raytracer;

public class Light {
	private Vector3 position, color;
	
	public Light(Vector3 position) {
		this.position = position;
		color = new Vector3(1.0, 1.0, 1.0);
	}
	
	public Light(Vector3 position, Vector3 color) {
		this.position = position;
		this.color = color;
	}
	
	public Vector3 getPosition() {
		return position;
	}
	
	public Vector3 getColor() {
		return color;
	}
}
