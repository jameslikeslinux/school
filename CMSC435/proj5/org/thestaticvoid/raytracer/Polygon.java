/*
 * Polygon.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Polygon scene object made up of several vertices on a plane.
 */
package org.thestaticvoid.raytracer;

import java.util.*;

public class Polygon extends SceneObject {
	protected List<Vector3> vertices;
	protected Vector3 normal;
	
	public Polygon(Scene scene) {
		super(scene);
		vertices = new ArrayList<Vector3>();
	}
	
	public void addVertex(Vector3 vertex) {
		vertices.add(vertex);
		
		// when we have enough vertices, calculate normal
		// (this assums the vertices don't all lie on a line
		if (vertices.size() == 3) {
			Vector3 a = vertices.get(1).subtract(vertices.get(0));
			Vector3 b = vertices.get(2).subtract(vertices.get(0));
			normal = a.crossProduct(b).normalize();
		}
	}
	
	public Vector3 getNormal(Intersection intersection) {
		return normal;
	}
	
	public Intersection intersects(Vector3 rayPosition, Vector3 rayDirection) {
		/*
		 * Basic line-plane intersection as found in the slides at
		 * http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
		 */
		double denominator = rayDirection.dotProduct(normal);
		if (denominator == 0)
			return null;

		double t = (vertices.get(0).dotProduct(normal) - rayPosition.dotProduct(normal)) / denominator;
		if (t < 0)
			return null;

		Vector3 intersectionPoint = rayPosition.add(rayDirection.scale(t));

//		/*
//		 * The following commented code is based on
//		 * http://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld018.htm
//		 * but only works for convex polygons.
//		 */
//		for (int i = 0; i < vertices.size(); i++) {
//			Vector3 v1 = vertices.get(i).subtract(intersection);
//			Vector3 v2 = vertices.get((i + 1) % vertices.size()).subtract(intersection);
//			Vector3 n1 = v2.crossProduct(v1).normalize();
//			double d1 = -rayPosition.dotProduct(n1);
//			if (intersection.dotProduct(n1) + d1 < 0)
//				return NO_INTERSECTION;
//		} 
//		
//		return t;
		
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
		
		Vector2 intersection2d = intersectionPoint.toVector2(dropComponent);
		
		Vector2[] vertices2d = new Vector2[vertices.size()];
		for (int i = 0; i < vertices.size(); i++)
			vertices2d[i] = vertices.get(i).toVector2(dropComponent);
		
		/* 
		 * Point-in-complex-polygon test from http://www.visibone.com/inpoly/
		 */
		boolean inside = false;		
		double xold = vertices2d[vertices2d.length - 1].getX();
		double yold = vertices2d[vertices2d.length - 1].getY();
		double x1, y1, x2, y2;
		
		for (Vector2 vertex2d : vertices2d) {
			double xnew = vertex2d.getX();
			double ynew = vertex2d.getY();
			
			if (xnew > xold) {
				x1 = xold;
				x2 = xnew;
				y1 = yold;
				y2 = ynew;
			} else {
				x1 = xnew;
				x2 = xold;
				y1 = ynew;
				y2 = yold;
			}
			
			if ((xnew < intersection2d.getX()) == (intersection2d.getX() <= xold) &&
					(intersection2d.getY() - y1) * (x2 - x1) < (y2 - y1) * (intersection2d.getX() - x1))
				inside = !inside;
			
			xold = xnew;
			yold = ynew;
		}
		
		if (inside)
			return new Intersection(this, t, intersectionPoint, rayDirection);
		
		return null; 
	}
}
