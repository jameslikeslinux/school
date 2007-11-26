/*
 * PolygonalPatch.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Extends polygon for interpolation of per-vertex normals.
 */
package org.thestaticvoid.raytracer;

import java.util.*;

public class PolygonalPatch extends Polygon {
	private List<Vector3> normals;
	
	public PolygonalPatch(Scene scene) {
		super(scene);
		normals = new ArrayList<Vector3>();
	}
	
	public void addVertex(Vector3 vertex) {
		addVertex(vertex, new Vector3(0.0));
	}
	
	public void addVertex(Vector3 vertex, Vector3 normal) {
		super.addVertex(vertex);
		normals.add(normal);
	}
	
	private static double cotangent(Vector3 a, Vector3 b, Vector3 c) {
		Vector3 ba = a.subtract(b);
		Vector3 bc = c.subtract(b);
		return bc.dotProduct(ba) / bc.crossProduct(ba).length();
	}
	
	public Vector3 getNormal(Intersection intersection) {
		/* 
		 * Generalized barycentric interpolation from
		 * www.geometry.caltech.edu/pubs/MHBD02.pdf
		 */
		Vector3 intersectionPoint = intersection.getPoint();
		double[] weights = new double[vertices.size()];
		double weightSum = 0.0;
		
		for (int i = 0; i < vertices.size(); i++) {
			Vector3 vertex = vertices.get(i);
			int prev = (i + vertices.size() - 1) % vertices.size();
			int next = (i + 1) % vertices.size();
			weights[i] = (cotangent(intersectionPoint, vertex, vertices.get(prev)) + cotangent(intersectionPoint, vertex, vertices.get(next))) / Math.pow(intersectionPoint.subtract(vertex).length(), 2);
			weightSum += weights[i];
		}
		
		for (int i = 0; i < weights.length; i++)
			weights[i] /= weightSum;
		
		Vector3 interpolatedNormal = new Vector3(0.0);
		for (int i = 0; i < weights.length; i++)
			interpolatedNormal = interpolatedNormal.add(normals.get(i).scale(weights[(i + 3) % weights.length]));
		
		return interpolatedNormal.normalize();
	}
}
