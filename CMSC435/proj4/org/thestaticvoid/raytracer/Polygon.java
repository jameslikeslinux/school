/*
 * Polygon.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Polygon scene object made up of several vertices on a plane.
 */
package org.thestaticvoid.raytracer;

import java.util.*;

public class Polygon implements SceneObject {
	private Vector3 fillColor;
	private List<Vector3> vertices;
	private Vector3 normal;
	
	public Polygon(Vector3 fillColor) {
		this.fillColor = fillColor;
		this.vertices = new ArrayList<Vector3>();
	}
	
	public void addVertex(Vector3 vertex) {
		vertices.add(vertex);
		
		// when we have enough vertices, calculate normal
		// (this assums the vertices don't all lie on a line
		if (vertices.size() == 3) {
			Vector3 a = vertices.get(0).subtract(vertices.get(1));
			Vector3 b = vertices.get(2).subtract(vertices.get(1));
			normal = a.crossProduct(b).normalize();
		}
	}

	public Vector3 getColor() {
		return fillColor;
	}
	
	public double intersects(Vector3 rayPosition, Vector3 rayDirection) {
		/*
		 * Basic line-plane intersection as found in the slides at
		 * http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
		 */
		double denominator = rayDirection.dotProduct(normal);
		if (denominator == 0)
			return NO_INTERSECTION;

		double t = (vertices.get(0).dotProduct(normal) - rayPosition.dotProduct(normal)) / denominator;
		if (t < 0)
			return NO_INTERSECTION;

		Vector3 intersection = rayPosition.add(rayDirection.scale(t));

		/*
		 * The following commented code is based on
		 * http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld018.htm
		 * but only works for convex polygons.
		 */
		/* for (int i = 0; i < vertices.size(); i++) {
			Vector3 v1 = vertices.get(i).subtract(intersection);
			Vector3 v2 = vertices.get((i + 1) % vertices.size()).subtract(intersection);
			Vector3 n1 = v2.crossProduct(v1).normalize();
			double d1 = -rayPosition.dotProduct(n1);
			if (intersection.dotProduct(n1) + d1 < 0)
				return NO_INTERSECTION;
		} 
		
		return t; */
		
		/*
		 * This code, however, works on all complex polygons.
		 */
		Vector3.Component dropComponent;
		
		if (normal.getX() > normal.getY() && normal.getX() > normal.getZ())
			dropComponent = Vector3.Component.X;
		else if (normal.getY() > normal.getX() && normal.getY() > normal.getZ())
			dropComponent = Vector3.Component.Y;
		else
			dropComponent = Vector3.Component.Z;
		
		Vector2 intersection2d = intersection.toVector2(dropComponent);
		
		List<Vector2> vertices2d = new ArrayList<Vector2>();
		for (Vector3 vertex : vertices)
			vertices2d.add(vertex.toVector2(dropComponent));
		
		/* 
		 * the rest of the code in this function is based on code in the
		 * public domain:
		 * http://alienryderflex.com/polygon/
		 */
		boolean inside = false;

		for (int i = 0, j = vertices2d.size() - 1; i < vertices2d.size(); j = i++) {
			Vector2 vertexi = vertices2d.get(i);
			Vector2 vertexj = vertices2d.get(j);

			if (vertexi.getY() < intersection2d.getY() && vertexj.getY() >= intersection2d.getY() ||
					vertexj.getY() < intersection2d.getY() && vertexi.getY() >= intersection2d.getY())
				if (vertexi.getX() + (intersection2d.getY() - vertexi.getY()) / (vertexj.getY() - vertexi.getY()) * (vertexj.getX() - vertexi.getX()) < intersection2d.getX())
					inside = !inside;

		}

		if (inside)
			return t;
		
		return NO_INTERSECTION; 
	}
}
